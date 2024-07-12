#include "crtti/base/c_static_assert.h"

namespace ncore
{
    namespace nrtti
    {
        RTTR_INLINE type_info_t::type_info_t()
            : m_id(0)
        {
        }

        RTTR_INLINE type_info_t::type_info_t(type_info_t::type_id_t id)
            : m_id(id)
        {
        }

        RTTR_INLINE type_info_t::type_info_t(const type_info_t &other)
            : m_id(other.m_id)
        {
        }

        RTTR_INLINE type_info_t &type_info_t::operator=(const type_info_t &other)
        {
            m_id = other.m_id;
            return *this;
        }

        RTTR_INLINE bool type_info_t::operator<(const type_info_t &other) const { return (m_id < other.m_id); }
        RTTR_INLINE bool type_info_t::operator>(const type_info_t &other) const { return (m_id > other.m_id); }
        RTTR_INLINE bool type_info_t::operator>=(const type_info_t &other) const { return (m_id >= other.m_id); }
        RTTR_INLINE bool type_info_t::operator<=(const type_info_t &other) const { return (m_id <= other.m_id); }
        RTTR_INLINE bool type_info_t::operator==(const type_info_t &other) const { return (m_id == other.m_id); }
        RTTR_INLINE bool type_info_t::operator!=(const type_info_t &other) const { return (m_id != other.m_id); }
        RTTR_INLINE type_info_t::type_id_t type_info_t::getId() const { return m_id; }
        RTTR_INLINE bool                type_info_t::isValid() const { return (m_id != 0); }

        namespace impl
        {

            template <typename T>
            struct metatype_info_t
            {
                enum
                {
                    Defined = 0
                };
                static type_info_t getTypeInfo()
                {
                    // when you get this error, you have to declare first the type with this macro
                    return T::TYPE_NOT_REGISTERED__USE_RTTR_DECLARE_META_TYPE();
                }
            };

            template <typename T>
            struct auto_register_type_t;

            template <typename T>
            static RTTR_INLINE type_info_t getTypeInfoFromInstance(const T *)
            {
                return impl::metatype_info_t<T>::getTypeInfo();
            }

            template <typename T, bool = Traits::is_same<T, typename raw_type<T>::type>::value>
            struct raw_type_info_t
            {
                static RTTR_INLINE type_info_t get() { return type_info_t(); }  // we have to return an empty type_info_t, so we can stop the recursion
            };

            template <typename T>
            struct raw_type_info_t<T, false>
            {
                static RTTR_INLINE type_info_t get() { return metatype_info_t<typename raw_type<T>::type>::getTypeInfo(); }
            };

            /*!
             * Determine if the given type \a T has the method
             * 'type_info_t getTypeInfo() const' declared.
             */
            template <typename T>
            class has_getTypeInfo_func_impl
            {
                typedef char YesType[1];
                typedef char NoType[2];

                template <typename U, nrtti::type_info_t (U::*)() const>
                class check
                {
                };

                template <typename C>
                static YesType &f(check<C, &C::getTypeInfo> *);

                template <typename C>
                static NoType &f(...);

            public:
                static const bool value = (sizeof(f<typename raw_type<T>::type>(0)) == sizeof(YesType));
            };

            /*!
             * If T has a member function 'type_info_t getTypeInfo() const;' then inherits from true_type, otherwise inherits from false_type.
             */
            template <class T, class = void>
            struct has_getTypeInfo_func : Traits::integral_constant<bool, false>
            {
            };

            template <class T>
            struct has_getTypeInfo_func<T, typename enable_if<has_getTypeInfo_func_impl<T>::value>::type> : Traits::integral_constant<bool, true>
            {
            };

            template <typename T, bool>
            struct TypeInfoFromInstance;

            //! Specialization for retrieving the type_info_t from the instance directly
            template <typename T>
            struct TypeInfoFromInstance<T, false>  // the typeInfo function is not available
            {
                static RTTR_INLINE type_info_t get(T &) { return impl::metatype_info_t<typename Traits::remove_cv<typename Traits::remove_reference<T>::type>::type>::getTypeInfo(); }
            };

            //! Specialization for retrieving the type_info_t from the instance directly
            template <typename T>
            struct TypeInfoFromInstance<T, true>
            {
                static RTTR_INLINE type_info_t get(T &object) { return object.getTypeInfo(); }
            };

        }  // end namespace impl

        template <typename T>
        RTTR_INLINE type_info_t type_info_t::get()
        {
            return impl::metatype_info_t<typename Traits::remove_cv<typename Traits::remove_reference<T>::type>::type>::getTypeInfo();
        }

        template <typename T>
        RTTR_INLINE type_id_t type_info_t::get_id()
        {
            type_info_t info = get();
            return info.getId();
        }

        template <typename T>
        RTTR_INLINE type_info_t type_info_t::get(T *)
        {
            return impl::metatype_info_t<T *>::getTypeInfo();
        }

        template <typename T>
        RTTR_INLINE type_info_t type_info_t::get(T &object)
        {
            return impl::TypeInfoFromInstance<T, impl::has_getTypeInfo_func<T>::value>::get(object);
        }

        template <typename T>
        RTTR_INLINE bool type_info_t::isTypeDerivedFrom() const
        {
            return isTypeDerivedFrom(impl::metatype_info_t<T>::getTypeInfo());
        }

    }  // end namespace nrtti
}  // namespace ncore

#define RTTR_CAT_IMPL(a, b) a##b
#define RTTR_CAT(a, b)      RTTR_CAT_IMPL(a, b)

#define RTTR_DECLARE_META_TYPE(T)                                                                                     \
    namespace ncore                                                                                                   \
    {                                                                                                                 \
        namespace nrtti                                                                                               \
        {                                                                                                             \
            namespace impl                                                                                            \
            {                                                                                                         \
                template <>                                                                                           \
                struct metatype_info_t<T>                                                                             \
                {                                                                                                     \
                    enum                                                                                              \
                    {                                                                                                 \
                        Defined = 1                                                                                   \
                    };                                                                                                \
                    static RTTR_INLINE nrtti::type_info_t getTypeInfo()                                               \
                    {                                                                                                 \
                        int const   maximum = 32;                                                                     \
                        type_info_t outArray[maximum];                                                                \
                        int         i = 0;                                                                            \
                        base_classes<T>::retrieve(outArray, i, maximum);                                              \
                        static const type_info_t val = registerOrGetType(#T, raw_type_info_t<T>::get(), outArray, i); \
                        return val;                                                                                   \
                    }                                                                                                 \
                };                                                                                                    \
            }                                                                                                         \
        }                                                                                                             \
    }

#define RTTR_DEFINE_META_TYPE(T)                                              \
    namespace ncore                                                           \
    {                                                                         \
        namespace nrtti                                                       \
        {                                                                     \
            namespace impl                                                    \
            {                                                                 \
                template <>                                                   \
                struct auto_register_type_t<T>                                    \
                {                                                             \
                    auto_register_type_t() { metatype_info_t<T>::getTypeInfo(); } \
                };                                                            \
            }                                                                 \
        }                                                                     \
    }                                                                         \
    static const ncore::nrtti::impl::auto_register_type_t<T> RTTR_CAT(autoRegisterType, __COUNTER__);

#define RTTR_DECLARE_STANDARD_META_TYPE_VARIANTS(T) \
    RTTR_DECLARE_META_TYPE(T)                       \
    RTTR_DECLARE_META_TYPE(T *)                     \
    RTTR_DECLARE_META_TYPE(const T *)

#define RTTR_DEFINE_STANDARD_META_TYPE_VARIANTS(T) \
    RTTR_DEFINE_META_TYPE(T)                       \
    RTTR_DEFINE_META_TYPE(T *)                     \
    RTTR_DEFINE_META_TYPE(const T *)
