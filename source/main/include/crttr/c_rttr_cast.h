#ifndef __CRTTR_C_RTTR_CAST_H__
#define __CRTTR_C_RTTR_CAST_H__

/*!
 * \brief Casts the given \a object to type \a T.
 *
 * Returns the given object cast to type T if the object is of type T (or of a subclass); 
 * otherwise returns 0. If object is 0 then it will also return 0.
 *
 * \return
 */
template<typename T, typename Arg>
T rttr_cast(Arg object);

#include "crttr/impl/c_rttr_cast_impl.h"

#endif // __RTTR_RTTRCAST_H__
