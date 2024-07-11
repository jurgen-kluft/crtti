#ifndef __CRTTR_BASE_C_RAW_TYPE_H__
#define __CRTTR_BASE_C_RAW_TYPE_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    namespace nrtti
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
        }  // end namespace impl
    }  // end namespace nrtti
}  // namespace ncore

#endif  // __RTTR_RAWTYPE_H__
