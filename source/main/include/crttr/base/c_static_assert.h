#ifndef __CRTTR_BASE_C_STATIC_ASSERT_H__
#define __CRTTR_BASE_C_STATIC_ASSERT_H__

namespace RTTR
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
}  // end namespace RTTR

#define RTTR_STATIC_ASSERT(x, msg)                     \
    {                                                  \
        RTTR::impl::CompileTimeError<(x)> ERROR_##msg; \
        (void)ERROR_##msg;                             \
    }

#endif  // __RTTR_STATICASSERT_H__
