#ifndef __CRTTR_C_TYPE_INFO_H__
#define __CRTTR_C_TYPE_INFO_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "crtti/base/c_core_prerequisites.h"
#include "crtti/base/c_type_traits.h"

namespace ncore
{
    namespace nrtti
    {
        class type_info_t;

        namespace impl
        {
            /*!
             * \brief Register the type info for the given name
             *
             * \remark When a type with the given name is already registered,
             *         then the type_info_t for the already registered type will be returned.
             *
             * \return A valid type_info_t object.
             */
            RTTR_API type_info_t registerOrGetType(const char *name, const type_info_t &rawTypeInfo, type_info_t const *info, int numBaseClasses);

            template <typename T, bool>
            struct raw_type_info_t;
        }  // end namespace impl

        /*!
         * This class holds the type information for any arbitrary object.
         *
         * Every class or atomic data type can have an unique type_info_t object.
         * With the help of this object you can compare unknown types for equality at runtime.
         *
         * Preparation
         * -----------
         * Before you can retrieve data from type_info_t, you have to register your struct or class.
         * Therefore use the macro #RTTR_DECLARE_META_TYPE(Type) to make the type known to the type_info_t system.
         * To actual execute the registration process use the macro #RTTR_DEFINE_META_TYPE(Type) in global namespace.
         *
         * This example shows a typical usage:
        \code{.cpp}
          // MyStruct.h
          struct MyStruct
          {
            int i;
          };

          RTTR_DECLARE_META_TYPE(MyStruct)

          // MyStruct.cpp; in global namespace
          RTTR_DEFINE_META_TYPE(MyStruct)
        \endcode
         *
         * Retrieve %type_info_t
         * ------------------
         * There are three static template member functions for retrieving the type_info_t:
         *
         *  type_info_t::get<T>()
              \code{.cpp}
                type_info_t::get<int>() == type_info_t::get<int>()  // yields to true
                type_info_t::get<int>() == type_info_t::get<bool>() // yields to false
              \endcode
         *  type_info_t::get<T>(T* ptr)
             \code{.cpp}

                struct Base {};
                struct Derived : Base {};
                struct Other : Base {};
                Derived d;
                Base* base = &d;
                type_info_t::get<Derived>() == type_info_t::get(base) // yields to true

                Other o;
                base = &o;
                type_info_t::get<Derived>() == type_info_t::get(base) // yields to false
             \endcode
         *  type_info_t::get<T>(T& ref)
             \code{.cpp}
                Derived d;
                type_info_t::get<Derived>() == type_info_t::get(d) // yields to true
             \endcode
         *
         */
        class RTTR_API type_info_t
        {
        public:
            typedef u16 type_id_t;

            /*!
             * Constructs an empty and invalid type_info_t object.
             */
            type_info_t();

            /*!
             * \brief Assigns a type_info_t to another one.
             *
             */
            type_info_t(const type_info_t &other);

            /*!
             * \brief Assigns a type_info_t to another one.
             *
             * \return A type_info_t object.
             */
            type_info_t &operator=(const type_info_t &other);

            /*!
             * \brief Comparison operator for sorting the type_info_t data according to some internal criterion.
             *
             * \return True if this type_info_t is less than the \a other.
             */
            bool operator<(const type_info_t &other) const;

            /*!
             * \brief Comparison operator for sorting the type_info_t data according to some internal criterion.
             *
             * \return True if this type_info_t is greater than the \a other.
             */
            bool operator>(const type_info_t &other) const;

            /*!
             * \brief Comparison operator for sorting the type_info_t data according to some internal criterion.
             *
             * \return True if this type_info_t is greater than or equal to \a other.
             */
            bool operator>=(const type_info_t &other) const;

            /*!
             * \brief Comparison operator for sorting the type_info_t data according to some internal criterion.
             *
             * \return True if this type_info_t is less than or equal to \a other.
             */
            bool operator<=(const type_info_t &other) const;

            /*!
             * \brief Compares this type_info_t with the \a other type_info_t and returns true
             *        if both describe the same type, otherwise returns false.
             *
             * \return True if both type_info_t are equal, otherwise false.
             */
            bool operator==(const type_info_t &other) const;

            /*!
             * \brief Compares this type_info_t with the \a other type_info_t and returns true
             *        if both describe different types, otherwise returns false.
             *
             * \return True if both type_info_t are \b not equal, otherwise false.
             */
            bool operator!=(const type_info_t &other) const;

            /*!
             * \brief Returns the id of this type.
             *
             * \note This id is unique at process runtime,
             *       but the id can be changed every time the process is executed.
             *
             * \return The type_info_t id.
             */
            type_id_t getId() const;

            /*!
             * \brief Returns the unique and human-readable name of the type.
             *
             * \return type_info_t name.
             */
            const char *getName() const;

            /*!
             * \brief Returns true if this type_info_t is valid, that means the type_info_t holds valid data to a type.
             *
             * \return True if this type_info_t is valid, otherwise false.
             */
            bool isValid() const;

            /*!
             * \brief Returns true if this type_info_t is derived from the given type \a T, otherwise false.
             *
             * \return Returns true if this type_info_t is a derived type from \a T, otherwise false.
             */
            template <typename T>
            bool isTypeDerivedFrom() const;

            /*!
             * \brief Returns a type_info_t object which represent the raw type.
             *
             * That means a the type without any qualifiers (const and volatile) nor any pointer.
             *
             * \remark When the current type_info_t is already the raw type, it will return an copy from itself.
             *
             * \return The type_info_t of the raw type.
             */
            type_info_t getRawType() const;

            template <typename T>
            static type_info_t get();

            template <typename T>
            static type_id_t get_id();

            template <typename T>
            static type_info_t get(T *object);

            /*!
             * \brief Returns a type_info_t object for the given instance \a object.
             *
             * \remark When type_info_t::get is applied to a glvalue expression whose type is a polymorphic class type,
             *         the result refers to a type_info_t object representing the type of the most derived object.
             \code{.cpp}
              class D { ... };
              D d1;
              const D d2;
              type_info_t::get(d1)  == type_info_t::get(d2);         // yields true
              type_info_t::get<D>() == type_info_t::get<const D>();  // yields true
              type_info_t::get<D>() == type_info_t::get(d2);         // yields true
              type_info_t::get<D>() == type_info_t::get<const D&>(); // yields true
             \endcode
             *
             * \return type_info_t for an \a object of type \a T.
             */
            template <typename T>
            static type_info_t get(T &object);

        private:
            /*!
             * \brief Constructs a valid type_info_t object.
             *
             * \param id The unique id of the data type.
             */
            type_info_t(type_id_t id);

            /*!
             * \brief Returns true if this type_info_t is derived from the given type_info_t \a other, otherwise false.
             *
             * \return Returns true if this type_info_t is a derived type from \a other, otherwise false.
             */
            bool isTypeDerivedFrom(const type_info_t &other) const;

            RTTR_API friend type_info_t impl::registerOrGetType(const char *name, const type_info_t &rawTypeInfo, type_info_t const *info, int numBaseClasses);
            template <typename T, bool>
            friend struct impl::raw_type_info_t;

        private:
            type_id_t m_id;
        };

#ifdef DOXYGEN
/*!
 * This macro makes the type \p Type known to the nrtti::type_info_t system.
 *
 * The macro should be placed directly under declaration of the custom class or struct of \p Type.
 * So nrtti::type_info_t can access it in every translation unit (*.cpp file).
 *
 * When using a \p Type without this registration, it will lead to a compile time error
 * with following message 'Type::TYPE_NOT_REGISTERED__USE_RTTR_DECLARE_META_TYPE'
 *
 * The following example will demonstrate the usage:
 \code{.cpp}
 // MyStruct.h
 struct MyStruct
 {
   bool visible;
 };

 RTTR_DECLARE_META_TYPE(MyStruct)
 \endcode
 *
 * When MyStruct is in a namespace, make sure you putt the macro outside the namespace,
 * otherwise nrtti::type_info_t cannot access the \p Type.
 \code{.cpp}
 namespace NSMyStruct
 {
 }

 RTTR_DECLARE_META_TYPE(NSMyStruct::MyStruct)
 \endcode
 */
#    define RTTR_DECLARE_META_TYPE(Type)

/*!
 * This macro makes the type \p Type immediately known to the nrtti::type_info_t system.
 * Place this macro inside the global namespace of one translation unit. Normally it is placed
 * inside the corresponding cpp file of type \p Type.
 *
 * The reason for this macro is, to make sure there is no race condition during the registration process
 * and on the other side to make sure that the registration process has finished before main was executed.
 \code{.cpp}
 // MyStruct.cpp
 RTTR_DEFINE_META_TYPE(MyStruct)
 \endcode
 */
#    define RTTR_DEFINE_META_TYPE(Type)
#endif

    }  // end namespace nrtti
}  // namespace ncore

#include "crtti/impl/c_type_info_impl.h"

#endif  // __RTTR_TYPEINFO_H__
