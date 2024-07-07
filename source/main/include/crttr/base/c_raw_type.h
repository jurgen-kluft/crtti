#ifndef __CRTTR_BASE_C_RAW_TYPE_H__
#define __CRTTR_BASE_C_RAW_TYPE_H__

namespace RTTR
{
    namespace impl
    {
        template <class T>
        struct raw_type
        {
            typedef T type;
        };

        template <class T>
        struct raw_type<const T>
        {
            typedef typename raw_type<T>::type type;
        };

        template <class T>
        struct raw_type<T *>
        {
            typedef typename raw_type<T>::type type;
        };
        template <class T>
        struct raw_type<T *const>
        {
            typedef typename raw_type<T>::type type;
        };
        template <class T>
        struct raw_type<T *volatile>
        {
            typedef typename raw_type<T>::type type;
        };

        template <class T>
        struct raw_type<T &>
        {
            typedef typename raw_type<T>::type type;
        };
    } // end namespace impl

} // end namespace RTTR

#endif // __RTTR_RAWTYPE_H__
