#ifndef __CRTTR_TYPE_TRAITS_H__
#define __CRTTR_TYPE_TRAITS_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "crttr/base/c_raw_type.h"
#include <type_traits>

namespace ncore
{
    namespace nrtti
    {

#if RTTR_COMPILER == RTTR_COMPILER_MSVC
#    if RTTR_COMP_VER > 1500
        namespace Traits = std;
#    elif RTTR_COMP_VER == 1500
        namespace Traits = std::tr1;
#    else
#        error "No type_traits for this Visual Studio version available! Please upgrade Visual Studio or use Boost."
#    endif
#else
        namespace Traits = std;
#endif

        // This is an own enable_if implementation, so we can reuse it with boost::integral_constant and std::integral_constant.
        namespace impl
        {
            template <bool B, class T = void>
            struct enable_if
            {
            };

            template <class T>
            struct enable_if<true, T>
            {
                typedef T type;
            };
        }  // end namespace impl

    }  // end namespace nrtti
}  // namespace ncore

#endif  // __RTTR_TYPETRAITS_H__
