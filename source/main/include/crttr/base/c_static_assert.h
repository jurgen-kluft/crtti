#ifndef __CRTTR_BASE_C_STATIC_ASSERT_H__
#define __CRTTR_BASE_C_STATIC_ASSERT_H__
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
            template <bool>
            struct CompileTimeError;
            template <>
            struct CompileTimeError<true>
            {
            };  // only true is defined

        }  // end namespace impl
    }  // end namespace nrtti
}  // namespace ncore

#define RTTR_STATIC_ASSERT(x, msg)                            \
    {                                                         \
        ncore::nrtti::impl::CompileTimeError<(x)> ERROR_##msg; \
        (void)ERROR_##msg;                                    \
    }

#endif  // __RTTR_STATICASSERT_H__
