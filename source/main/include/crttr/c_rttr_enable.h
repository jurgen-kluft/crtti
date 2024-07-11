#ifndef __CRTTR_C_RTTR_ENABLE_H__
#define __CRTTR_C_RTTR_ENABLE_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "crttr/base/c_type_traits.h"
#include "crttr/base/c_static_assert.h"

namespace ncore
{
    namespace nrtti
    {
        namespace impl
        {
            //! A simple typelist_t
            struct nil
            {
            };
            template <class T, class U = nil>
            struct typelist_t
            {
                typedef T head;
                typedef U tail;
            };

            /////////////////////////////////////////////////////////////////////////////////////
            /*!
             * This trait checks if a given type T has a typedef named \a baseClassList.
             * has_base_class_list_impl::value is true, when it has this type, otherwise false.
             */
            template <typename T>
            class has_base_class_list_impl
            {
                typedef char YesType[1];
                typedef char NoType[2];

                template <typename C>
                static YesType& test(typename C::baseClassList*);

                template <typename>
                static NoType& test(...);

            public:
                static const bool value = (sizeof(YesType) == sizeof(test<T>(0)));
            };

            /*!
             * If T has a typedef called \a 'baseClassList' then inherits from true_type, otherwise inherits from false_type.
             */
            template <class T, class = void>
            struct has_base_class_list : Traits::integral_constant<bool, false>
            {
            };

            template <class T>
            struct has_base_class_list<T, typename enable_if<has_base_class_list_impl<T>::value>::type> : Traits::integral_constant<bool, true>
            {
            };

            /*!
             * This class fills from a given typelist_t the corresponding type_info_t objects into a std::vector.
             */
            template <class>
            struct typeinfo_from_baseclass_list_t;

            template <>
            struct typeinfo_from_baseclass_list_t<typelist_t<nil>>
            {
                static RTTR_INLINE void fill(type_info_t* outArray, int& i, int maximum) {}
            };

            template <class T, class U>
            struct typeinfo_from_baseclass_list_t<typelist_t<T, U>>
            {
                static RTTR_INLINE void fill(type_info_t* outArray, int& i, int maximum)
                {
                    RTTR_STATIC_ASSERT(has_base_class_list<T>::value, PARENT_CLASS_HAS_NO_BASE_CLASS_LIST_DEFINIED__USE_RTTR_ENABLE);
                    outArray[i++] = metatype_info_t<T>::getTypeInfo();

                    // retrieve also the type_info_t of all base class of the base classes
                    typeinfo_from_baseclass_list_t<typename T::baseClassList>::fill(outArray, i, maximum);
                    typeinfo_from_baseclass_list_t<U>::fill(outArray, i, maximum);
                }
            };

            /*!
             * This helper trait returns a vector with type_info_t object of all base classes.
             * When there is no typelist_t defined or the class has no base class, an empty vector is returned.
             */
            template <class T>
            struct base_classes
            {
            private:
                // extract the info
                static RTTR_INLINE void retrieve_impl(type_info_t* outArray, int& i, int maximum, Traits::true_type) { typeinfo_from_baseclass_list_t<typename T::baseClassList>::fill(outArray, i, maximum); }

                // no type list defined
                static RTTR_INLINE void retrieve_impl(type_info_t* outArray, int& i, int maximum, Traits::false_type) {}

            public:
                static RTTR_INLINE void retrieve(type_info_t* outArray, int& i, int maximum) { retrieve_impl(outArray, i, maximum, typename has_base_class_list<T>::type()); }
            };

        }  // end namespace impl
    }  // end namespace nrtti
}  // namespace ncore

#define TYPE_LIST()                ncore::nrtti::impl::typelist_t<ncore::nrtti::impl::nil>
#define TYPE_LIST_1(A)             ncore::nrtti::impl::typelist_t<A, TYPE_LIST()>
#define TYPE_LIST_2(A, B)          ncore::nrtti::impl::typelist_t<A, TYPE_LIST_1(B)>
#define TYPE_LIST_3(A, B, C)       ncore::nrtti::impl::typelist_t<A, TYPE_LIST_2(B, C)>
#define TYPE_LIST_4(A, B, C, D)    ncore::nrtti::impl::typelist_t<A, TYPE_LIST_3(B, C, D)>
#define TYPE_LIST_5(A, B, C, D, E) ncore::nrtti::impl::typelist_t<A, TYPE_LIST_4(B, C, D, E)>

#define RTTR_ENABLE()                                                                                            \
public:                                                                                                          \
    virtual RTTR_INLINE ncore::nrtti::type_info_t getTypeInfo() const { return ncore::nrtti::impl::getTypeInfoFromInstance(this); } \
    typedef TYPE_LIST() baseClassList;                                                                           \
                                                                                                                 \
private:

#define RTTR_ENABLE_DERIVED_FROM(A)                                                                              \
public:                                                                                                          \
    virtual RTTR_INLINE ncore::nrtti::type_info_t getTypeInfo() const { return ncore::nrtti::impl::getTypeInfoFromInstance(this); } \
    typedef TYPE_LIST_1(A) baseClassList;                                                                        \
                                                                                                                 \
private:

#define RTTR_ENABLE_DERIVED_FROM_2(A, B)                                                                         \
public:                                                                                                          \
    virtual RTTR_INLINE ncore::nrtti::type_info_t getTypeInfo() const { return ncore::nrtti::impl::getTypeInfoFromInstance(this); } \
    typedef TYPE_LIST_2(A, B) baseClassList;                                                                     \
                                                                                                                 \
private:

#define RTTR_ENABLE_DERIVED_FROM_3(A, B, C)                                                                      \
public:                                                                                                          \
    virtual RTTR_INLINE ncore::nrtti::type_info_t getTypeInfo() const { return ncore::nrtti::impl::getTypeInfoFromInstance(this); } \
    typedef TYPE_LIST_3(A, B, C) baseClassList;                                                                  \
                                                                                                                 \
private:

#define RTTR_ENABLE_DERIVED_FROM_4(A, B, C, D)                                                                   \
public:                                                                                                          \
    virtual RTTR_INLINE ncore::nrtti::type_info_t getTypeInfo() const { return ncore::nrtti::impl::getTypeInfoFromInstance(this); } \
    typedef TYPE_LIST_4(A, B, C, D) baseClassList;                                                               \
                                                                                                                 \
private:

#define RTTR_ENABLE_DERIVED_FROM_5(A, B, C, D, E)                                                                \
public:                                                                                                          \
    virtual RTTR_INLINE ncore::nrtti::type_info_t getTypeInfo() const { return ncore::nrtti::impl::getTypeInfoFromInstance(this); } \
    typedef TYPE_LIST_5(A, B, C, D, E) baseClassList;                                                            \
                                                                                                                 \
private:

#endif  // __RTTR_RTTRENABLE_H__
