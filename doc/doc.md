Introduction[](#introduction)
-----------------------------

This doc will show how to implement an alternative to the built-in `RTTI` mechanism with just standard C++. The presented implementation has not the pretense to be a complete replacement for the C++ `RTTI` system. Rather it should improve certain issues which can appear when using `RTTI`. For instance, there are problems when using `RTTI` [across shared boundaries](https://svn.boost.org/trac/boost/ticket/754). Furthermore the alternative should be more performant, in terms of [execution speed](http://tinodidriksen.com/2010/04/14/cpp-dynamic-cast-performance/), than the standard `RTTI`.

The first part will show how to implement the `typeid` operator and a `type_info` class. In the second part an alternative to the `dynamic_cast` operator will be presented. Unnecessary implementation details are left out, only the general concept and the important parts are shown in detail.

RTTI[](#rtti)
-------------

`RTTI` stands for Run-Time Type Information. It is used to retrieve information about an object at runtime (as opposed to compile time). In non-polymorphic languages there is no need for this information, because the type is always known at compile time and so in runtime. In a polymorphic language like C++, there are situations where the concrete type is not known at compile time. A pointer can point to an object of a base class type or to an object of any class type derived from the base class Therefore `RTTI` helps to determine the concrete type at runtime.

Normally `RTTI` is an optional compiler setting, which is usually enabled. When enabled, `RTTI` is automatically generated for both built-in and non-polymorphic user-defined types. It is then not possible to disable/enable `RTTI` just for certain types selectively. In C++ the `RTTI` mechanism consists of:

* [`typeid`](http://en.cppreference.com/w/cpp/language/typeid) operator
* [`type_info`](http://en.cppreference.com/w/cpp/types/type_info) class
* [`dynamic_cast`](http://en.cppreference.com/w/cpp/language/dynamic_cast) operator

typeid and type_info[](#typeid_and_type_info)
---------------------------------------------

The `typeid` operator will be used to determine the dynamic type of an object at runtime. Therefore the `typeid` operator returns a constant reference to a `type_info` object. The programmer can then compare this object with other ones or retrieve the name.

When the `typeid` operator should work with polymorphic types, at least one virtual function has to be declared in the class. The reason for that is, the pointer to the `type_info` object is stored in the virtual table of the class. So `RTTI` will not increase the data per object, it will increase the data per class. It is not specified in the standard how big this metadata is.

In some special situation the typeid operator doesn't work, more precisely there are problems when used [across shared boundaries.](https://svn.boost.org/trac/boost/ticket/754) In C++ the concept of shared or dynamic libraries is not defined, so it is not a problem with the standard. Nevertheless, there exist a workaround for this problem:  
Using the `type_info::name` function for comparison instead of the built in compare operator will fix this issue, but this is of course slower. Another drawback is that the `type_info` objects are not copyable. That means it is not possible to use them directly in a container. Creating a [wrapper class](http://loki-lib.sourceforge.net/html/a00657.html "Lokis type_info wrapper") around `type_info` object will fix this issue.

Custom type_info[](#custom_type_info)
-------------------------------------

One of the standard approaches for implementing `typeid` is the following snippet:

```cpp
static int generateId()
{
    static int typeID; // automatically init with 0
    return ++typeID;
}
template <typename T>
struct MetaTypeInfo
{
    static int getTypeInfo()
    {
        static const int typeID = generateId();
        return typeID;
    };
};
```

For every type T that will instantiate the struct template MetaTypeInfo, it will return a unique integer from the function `getTypeInfo`.

Unfortunately this code will not work correctly across dynamic loaded libraries (DLL); it will return different integer values for the same type. In [windows](http://msdn.microsoft.com/en-us/library/h90dkhs0%28v=vs.80%29.aspx) every DLL gets its own copy of local and global static variables.

To work around this problem, a register functionality is needed, which uses the windows dllexport/dllimport attribute and returns for every type `T` always the same typeID. The presented solution in this article will use a unique string literal which correspond to the type `T`.

```cpp
template <typename T>
struct MetaTypeInfo
{
    static TypeInfo getTypeInfo()
    {
        // when you get this error,
        // you have to declare first the type with this macro
        return T::TYPE_NOT_REGISTERED__USE_RTTR_DECLARE_META_TYPE();
    }
}; // returns for every name a unique TypeInfo object

RTTR_EXPORT TypeInfo registerOrGetType(const char *name);

#define RTTR_DECLARE_META_TYPE(T)                              \
    template <>                                                \
    struct MetaTypeInfo<T>                                     \
    {                                                          \
        static RTTR::TypeInfo getTypeInfo()                    \
        {                                                      \
                                                               \
            static const TypeInfo val = registerOrGetType(#T); \
            return val;                                        \
        }                                                      \
    };

// use it like this:
RTTR_DECLARE_META_TYPE(int)
RTTR_DECLARE_META_TYPE(bool)
```


The integer value was replaced with the class `TypeInfo`, which holds the integer (ID) value. This class is the replacement for the `type_info` class. The template specialization for every supported type is required to register the given type `T` with its unique string literal, which will be extracted with the macro expression `#T`. The register function `registerOrGetType` maps the given string literal to a unique ID. When the key not yet exist in the map it will be stored together with the ID, otherwise the already existing ID will be returned. The Macro `RTTR_EXPORT` hides the dllexport/dllimport functionality required on windows.

The default implementation of `MetaTypeInfo<T>::getTypeInfo()` will now generate a compile time error, when a TypeInfo object should be retrieved for a type which has not be specialized. To avoid unnecessary rewrite work for every type it will be put inside the macro `RTTR_DECLARE_META_TYPE`.

This code has a serious issue; it is not thread-safe. There is a race condition during the initialization of the local static variable `val`. When one thread is executing the `registerOrGetType` function, another thread could already return the uninitialized value of `val`. There are several solutions to make the code thread-safe, one is to use the static initialization time, which is thread-safe. Therefore the following implementation uses a little helper class which automatically register a type before main is called.

```c++
#define RTTR_CAT_IMPL(a, b) a##b
#define RTTR_CAT(a, b) RTTR_CAT_IMPL(a, b)

#define RTTR_DEFINE_META_TYPE(T)            \
    template <>                             \
    struct AutoRegisterType<T>              \
    {                                       \
        AutoRegisterType()                  \
        {                                   \
            MetaTypeInfo<T>::getTypeInfo(); \
        }                                   \
    };                                      \
    static const AutoRegisterType<T> RTTR_CAT(autoRegisterType, __COUNTER__);
// __COUNTER__ gets incremented by 1 every time it is used in a source file

// use it like this:
RTTR_DEFINE_META_TYPE(int)
RTTR_DEFINE_META_TYPE(bool)
```

In general global variables should be avoided where possible, but in this particular case it is very handy to avoid any kind of locking. Furthermore the code which will be executed is very trivial and should not lead to any problems.

The `TypeInfo` class, which is the wrapper for the ID, provides also the interface for retrieving the `TypeInfo's` itself. They are declared as public member functions.

TypeInfo.h

```cpp
class RTTR_EXPORT TypeInfo
{
public:
    typedef unsigned short TypeId;

    // to use TypeInfo in variause std container
    // following functions are definied:
    TypeInfo(const TypeInfo &other);
    TypeInfo &operator=(const TypeInfo &other);
    bool operator<(const TypeInfo &other) const;
    bool operator>(const TypeInfo &other) const;
    bool operator>=(const TypeInfo &other) const;
    bool operator<=(const TypeInfo &other) const;
    bool operator==(const TypeInfo &other) const;
    bool operator!=(const TypeInfo &other) const;

    // Via following getter can a TypeInfo object retrieved:
    template <typename T>
    static TypeInfo get();

    template <typename T>
    static TypeInfo get(T *object);

    template <typename T>
    static TypeInfo get(T &object);
    //...
private:
    // Constructs an empty and invalid TypeInfo object.
    TypeInfo();
    // private to avoid creation by client
    TypeInfo(TypeId id);

    // this function can only create valid TypeInfo objects
    friend TypeInfo impl::registerOrGetType(const char *name);

private:
    TypeId m_id;
};
```

The client retrieves the `TypeInfo` object with a call to the static function `TypeInfo::get<T>()` when only the type is given or via a call to `TypeInfo::get(myInstance)` when an instance is given. A `TypeInfo` object can only be created with a call to these getters, that's why the constructors of TypeInfo are private.

These getters are implemented like defined in the standard (5.2.8.6):

> If the type of the expression or type-id is a cv-qualified type, the result of the typeid expression refers to a std::type_info object representing the cv-unqualified type.

Following example code demonstrate this behaviour:

```cpp
int intVar = 42;
const int constIntVar = 42;
TypeInfo::get(intVar) == TypeInfo::get(constIntVar);  // true
TypeInfo::get<int>() == TypeInfo::get(constIntVar);   // true
TypeInfo::get<int>() == TypeInfo::get<const int>();   // true
TypeInfo::get<int>() == TypeInfo::get<const int &>(); // true

TypeInfo::get<int *>() == TypeInfo::get(&intVar);                  // true
TypeInfo::get<int *>() == TypeInfo::get<int *const>();             // true
TypeInfo::get<const int *>() == TypeInfo::get(&constIntVar);       // true
TypeInfo::get<const int *>() == TypeInfo::get<const int *const>(); // true

const int &intConstRef = intVar;
TypeInfo::get<int>() == TypeInfo::get(intConstRef); // true
```

With the `typeid` operator it is possible to retrieve the `type_info` of the most derived type, although only a reference to a parent class is given. This is defined in following requirement (5.2.8.3):

> When typeid is applied to a glvalue expression whose type is a polymorphic class type (10.3), the result refers to a std::type_info object representing the type of the most derived object (1.8) (that is, the dynamic type) to which the glvalue refers.

In order to retrieve this information, the given class type must be polymorphic and so at least one virtual function must be defined. The reason for that is, the compiler will add an additional pointer (virtual pointer, vptr) in the virtual table (vtable) to the corresponding `type_info` object. Although the standard does not mention anything about virtual tables, it was intended to be implemented `RTTI` in the same way as virtual function are implemented.

With the help of virtual functions, it is possible to recreate this feature.

```cpp

template <typename T>
static TypeInfo getTypeInfoFromInstance(const T *)
{
    return MetaTypeInfo<T>::getTypeInfo();
}

struct Base
{
    virtual TypeInfo getTypeInfo() const
    {
        return getTypeInfoFromInstance(this);
    }
};

struct Derived : Base
{
    virtual TypeInfo getTypeInfo() const
    {
        return getTypeInfoFromInstance(this);
    }
};

Derived d;
Base b = d;
TypeInfo::get<D>() == TypeInfo::get(b);  // true
TypeInfo::get<D>() == TypeInfo::get(&b); // false

```

The expression `TypeInfo::get(b);` looks at compile time with the help of [SFINAE](http://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error) whether the given object has a member function `TypeInfo T::getTypeInfo() const` declared or not. When there is such a function with this name declared it will be called. Then the most derived member function implementation will be executed. Where finally the concrete type `T` can be retrieved. When no function is defined the `TypeInfo` of the current type will be returned. The virtual function is put again into a macro (`RTTR_ENABLE`) to avoid unnecessary rewrite work.

```cpp
#define RTTR_ENABLE() \
public:\
  virtual TypeInfo getTypeInfo() const {return getTypeInfoFromInstance(this);} \
private:

struct Base { RTTR_ENABLE() };
struct Derived : Base { RTTR_ENABLE() };
```

Like defined in the standard, only when `TypeInfo::get()` is called with a reference type, then the returned `TypeInfo` represents the concrete derived class type. When it is required to check against a target type, which is in the middle of the class hierarchy it won't work. Therefore, the use of `dynamic_cast` is the right tool.

dynamic_cast[](#dynamic_cast)
-----------------------------

The `dynamic_cast` operator is used to cast pointers or references to a target type using `RTTI`. Thus, the class itself must be polymorphic to allow the usage of `dynamic_cast`. When the types are incompatible a null pointer will be returned (when using pointers) or an exception will be thrown (when using references).

The client can perform three kinds of casts with `dynamic_cast`: upcasts, downcasts and crosscasts.  
The upcast is the operation of converting a derived class pointer or reference to its base (or parent) class. A downcast is the opposite, casting from a base class to a derived class. A crosscast means casting from one class hierarchy level to another. The involved classes has thereby only in common that they share the same derived class, they must not be related through inheritance.

In order to perform a cross cast the `dynamic_cast` operator needs to adjust the current base vptr to the vptr of the target type. This is done through offset calculations. The offsets to every other vptr in the class hierarchy is also stored in the vtable. The virtual table is not described by the standard, so it is not possible to obtain these offsets in a portable way. That is reason why this alternative `RTTI` does not support crosscast's or virtual inheritance. Therefore, `dynamic_cast` is still necessary.

Custom dynamic_cast[](#custom_dynamic_cast)
-------------------------------------------

Supporting upcasts and downcasts (without virtual inheritance), requires that the complete inheritance graph (DAG) needs to be stored. Retrieving this information automatically is not possible with C++. The client has to register these kinds of meta information by himself.

For storing this meta information a loki like type list was used. The client will again only use macros, so everything is hidden.

```cpp

struct nil
{
};
template <class T, class U = nil>
struct typelist
{
    typedef T head;
    typedef U tail;
};

#define TYPE_LIST() typelist<nil>
#define TYPE_LIST_1(A) typelist<A, TYPE_LIST()>
#define TYPE_LIST_2(A, B) typelist<A, TYPE_LIST_1(B)>
//...
#define RTTR_ENABLE_DERIVED_FROM(A)                                                \
public:                                                                            \
    virtual TypeInfo getTypeInfo() const { return getTypeInfoFromInstance(this); } \
    typedef TYPE_LIST_1(A) baseClassList;                                          \
                                                                                   \
private:

#define RTTR_ENABLE_DERIVED_FROM_2(A, B)                                           \
public:                                                                            \
    virtual TypeInfo getTypeInfo() const { return getTypeInfoFromInstance(this); } \
    typedef TYPE_LIST_2(A, B) baseClassList;                                       \
                                                                                   \
private:

// Example usage
struct Base1
{
    RTTR_ENABLE()
};
struct Base2
{
    RTTR_ENABLE()
};
struct Multiple : Base1, Base2
{
    RTTR_ENABLE_DERIVED_FROM_2(Base1, Base2)
};

```

Adding this meta information inside the class itself, will not increase the size of the class. The register process is also less error prone because the client can directly see from what class he derives.

The list of the base classes needs to be converted to its corresponding `TypeInfo` objects. This is done with following code:

```cpp
template <class>
struct TypeInfoFromBaseClassList;

/*!
 * This class fills from a given typelist the,
 * corresponding TypeInfo objects into a std::vector.
 */
template <>
struct TypeInfoFromBaseClassList<typelist<nil>>
{
    static void fill(std::vector<TypeInfo> &)
    {
    }
};

template <class T, class U>
struct TypeInfoFromBaseClassList<typelist<T, U>>
{
    static void fill(std::vector<TypeInfo> &v)
    {
        v.push_back(MetaTypeInfo<T>::getTypeInfo());
        TypeInfoFromBaseClassList<typename T::baseClassList>::fill(v);
        TypeInfoFromBaseClassList<U>::fill(v);
    }
};

/*!
 * This helper trait returns a vector with TypeInfo object of all base classes.
 * When there is no typelist named baseClassList defined,
 * an empty vector is returned.
 */
template <class T>
struct BaseClasses
{
private:
    // extract the info
    static void retrieve_impl(std::vector<TypeInfo> &v, Traits::true_type)
    {
        TypeInfoFromBaseClassList<typename T::baseClassList>::fill(v);
    }

    // no type list defined
    static void retrieve_impl(std::vector<TypeInfo> &v, Traits::false_type)
    {
    }

public:
    static std::vector<TypeInfo> retrieve()
    {
        std::vector<TypeInfo> result;
        // check with SFINAE whether a typedef for baseClassList is defined or not
        retrieve_impl(result, typename has_base_class_list<T>::type());
        return result;
    }
};

```

The `BaseClasses<T>::retrieve()` function returns from a given `T` a `std::vector<TypeInfo>`, which contains for every item in the typelist the corresponding `TypeInfo`. The recursive function `TypeInfoFromBaseClassList<T>::fill(std::vector<TypeInfo>&)` iterates through the typelist and fills the given vector with its corresponding `TypeInfo`.

The register function is adjusted to store `TypeInfos` for every type and its base classes, thus the function `MetaTypeInfo<T>::getTypeInfo()` is adjusted in the following way:

```cpp
#define RTTR_DECLARE_META_TYPE(T)                                                      \
    template <>                                                                        \
    struct MetaTypeInfo<T>                                                             \
    {                                                                                  \
        static RTTR::TypeInfo getTypeInfo()                                            \
        {                                                                              \
            static const TypeInfo val = registerOrGetType(#T, RawTypeInfo<T>::get(),   \
                                                          BaseClasses<T>::retrieve()); \
            return val;                                                                \
        }                                                                              \
    };

```

The `RawTypeInfo<T>::get()` returns for every given type `T` the `TypeInfo` of its raw type. A raw type is a type without any cv-qualifier nor any pointer or reference. This raw type is necessary because it is used for the mapping to the base class meta information. Without this raw type every type (e.g. a pointer type) would need such a mapping.

rttr_cast[](#rttr_cast)
-----------------------

The custom cast operator is called `rttr_cast`, its implementation is straightforward:  
It uses a public member function `TypeInfo::isTypeDerivedFrom` which returns true if the given target type is derived from the operand type, otherwise false. For the cast itself the implementation makes usage of `static_cast`, because when multiple inheritance is involved the vptr needs to be adjusted. The `static_cast` operator does this automatically at compile time, `reinterpret_cast` does not perform any vptr adjustment. Besides the already mentioned not supported crosscasts and virtual inheritance, the other difference to `dynamic_cast` is that only pointer types can be cast. The reason for that is, exceptions are not supported.

```cpp
template<typename T, typename Arg>
T rttr_cast(Arg object)
{
  using namespace RTTR::Traits;
  
  RTTR_STATIC_ASSERT(is_pointer<T>::value, RETURN_TYPE_MUST_BE_A_POINTER);
  RTTR_STATIC_ASSERT(is_pointer<Arg>::value, ARGUMENT_TYPE_MUST_BE_A_POINTER);
  RTTR_STATIC_ASSERT(RTTR::impl::has_getTypeInfo_func<Arg>::value, 
                     CLASS_HAS_NO_TYPEINFO_DEFINIED__USE_MACRO_ENABLE_RTTI);
  
  typedef typename remove_pointer<T>::type ReturnType;
  typedef typename remove_pointer<Arg>::type ArgType;
  RTTR_STATIC_ASSERT((is_const<ArgType>::value && is_const<ReturnType>::value)||
                    (!is_const<ArgType>::value && is_const<ReturnType>::value)||
                    (!is_const<ArgType>::value && !is_const<ReturnType>::value),
                     RETURN_TYPE_MUST_HAVE_CONST_QUALIFIER)
  if (object && object->getTypeInfo().template isTypeDerivedFrom<T>())
    return static_cast<T>(object);
  else
    return NULL;
}
```

Conclusion[](#conclusion)
-------------------------

Although the comparison of `rttr_cast` and `dynamic_cast` is not 100% fair, because the custom implemented operator does not implement all operation which `dynamic_cast` support, it is clearly visible that `rttr_cast` outperforms the built-in operator. Especially as deeper as the hierachy goes `rttr_cast` can show his strength. In one case `rttr_cast` does the cast nearly 5 times faster.

Advantage over RTTI

* faster then then the built-in `RTTI` (especially with deep hierachies)
* possible to activate the `RTTI` mechanism only on certain types
* no hacks needed when used across shared libraries
* the `TypeInfo` objects are ready to used in standard container (no wrapper objects needed)

Disadvantage over RTTI

* necessary to register the types manually
* when used in polymorphic classes, a macro needs to be placed inside the class
* linking an additonal library (no header only)
* no support for crosscasts and virtual inheritance (this may change)
