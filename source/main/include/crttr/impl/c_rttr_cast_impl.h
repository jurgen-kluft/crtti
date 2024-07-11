#include "crttr/c_type_info.h"
#include "crttr/base/c_type_traits.h"
#include "crttr/base/c_static_assert.h"

template <typename T, typename Arg>
RTTR_INLINE T rttr_cast(Arg object)
{
    using namespace ncore::RTTR::Traits;

    RTTR_STATIC_ASSERT(is_pointer<T>::value, RETURN_TYPE_MUST_BE_A_POINTER);
    RTTR_STATIC_ASSERT(is_pointer<Arg>::value, ARGUMENT_TYPE_MUST_BE_A_POINTER);
    RTTR_STATIC_ASSERT(ncore::RTTR::impl::has_getTypeInfo_func<Arg>::value, CLASS_HAS_NO_TYPEINFO_DEFINIED__USE_MACRO_ENABLE_RTTI);

    typedef typename remove_pointer<T>::type   ReturnType;
    typedef typename remove_pointer<Arg>::type ArgType;
    RTTR_STATIC_ASSERT((is_const<ArgType>::value && is_const<ReturnType>::value) || (!is_const<ArgType>::value && is_const<ReturnType>::value) || (!is_const<ArgType>::value && !is_const<ReturnType>::value), RETURN_TYPE_MUST_HAVE_CONST_QUALIFIER);
    if (object && object->getTypeInfo().template isTypeDerivedFrom<T>())
        return static_cast<T>(object);
    else
        return NULL;
}
