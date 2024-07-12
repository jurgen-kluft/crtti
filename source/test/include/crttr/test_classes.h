#ifndef __RTTR_TESTCLASSES_H__
#define __RTTR_TESTCLASSES_H__

#include "crtti/c_rttr.h"

#define CLASS(CLASS_NAME) struct CLASS_NAME { RTTR_ENABLE() virtual int getType() { return 0; } int dummyIntValue; }; \
    RTTR_DECLARE_STANDARD_META_TYPE_VARIANTS(CLASS_NAME)

#define CLASS_INHERIT(CLASS1, CLASS2) struct CLASS1 : CLASS2 { virtual int getType() { return 1; } RTTR_ENABLE_DERIVED_FROM(CLASS2) double dummyDoubleValue; };\
    RTTR_DECLARE_STANDARD_META_TYPE_VARIANTS(CLASS1)

#define CLASS_MULTI_INHERIT_2(CLASS1, CLASS2, CLASS3) struct CLASS1 : CLASS2, CLASS3 { virtual int getType() { return 1; } RTTR_ENABLE_DERIVED_FROM_2(CLASS2, CLASS3) bool dummyBoolValue; };\
    RTTR_DECLARE_STANDARD_META_TYPE_VARIANTS(CLASS1)

#define CLASS_MULTI_INHERIT_5(CLASS1, CLASS2, CLASS3, CLASS4, CLASS5, CLASS6) struct CLASS1 : CLASS2, CLASS3, CLASS4, CLASS5, CLASS6 { virtual int getType() { return 1; } RTTR_ENABLE_DERIVED_FROM_5(CLASS2, CLASS3, CLASS4, CLASS5, CLASS6) bool dummyBoolValue; };\
    RTTR_DECLARE_STANDARD_META_TYPE_VARIANTS(CLASS1)

/////////////////////////////////////////////////////////////////////////////////////////
// The following class structures has 7 hierarchy levels and is 5 classes wide;
// only single inheritance
/////////////////////////////////////////////////////////////////////////////////////////

CLASS(ClassSingleBase)
CLASS_INHERIT(ClassSingle1A, ClassSingleBase)
CLASS_INHERIT(ClassSingle2A, ClassSingle1A)
CLASS_INHERIT(ClassSingle3A, ClassSingle2A)
CLASS_INHERIT(ClassSingle4A, ClassSingle3A)
CLASS_INHERIT(ClassSingle5A, ClassSingle4A)
CLASS_INHERIT(ClassSingle6A, ClassSingle5A)

CLASS_INHERIT(ClassSingle1B, ClassSingleBase)
CLASS_INHERIT(ClassSingle2B, ClassSingle1B)
CLASS_INHERIT(ClassSingle3B, ClassSingle2B)
CLASS_INHERIT(ClassSingle4B, ClassSingle3B)
CLASS_INHERIT(ClassSingle5B, ClassSingle4B)
CLASS_INHERIT(ClassSingle6B, ClassSingle5B)

CLASS_INHERIT(ClassSingle1C, ClassSingleBase)
CLASS_INHERIT(ClassSingle2C, ClassSingle1C)
CLASS_INHERIT(ClassSingle3C, ClassSingle2C)
CLASS_INHERIT(ClassSingle4C, ClassSingle3C)
CLASS_INHERIT(ClassSingle5C, ClassSingle4C)
CLASS_INHERIT(ClassSingle6C, ClassSingle5C)

CLASS_INHERIT(ClassSingle1D, ClassSingleBase)
CLASS_INHERIT(ClassSingle2D, ClassSingle1D)
CLASS_INHERIT(ClassSingle3D, ClassSingle2D)
CLASS_INHERIT(ClassSingle4D, ClassSingle3D)
CLASS_INHERIT(ClassSingle5D, ClassSingle4D)
CLASS_INHERIT(ClassSingle6D, ClassSingle5D)

CLASS_INHERIT(ClassSingle1E, ClassSingleBase)
CLASS_INHERIT(ClassSingle2E, ClassSingle1E)
CLASS_INHERIT(ClassSingle3E, ClassSingle2E)
CLASS_INHERIT(ClassSingle4E, ClassSingle3E)
CLASS_INHERIT(ClassSingle5E, ClassSingle4E)
CLASS_INHERIT(ClassSingle6E, ClassSingle5E)

/////////////////////////////////////////////////////////////////////////////////////////
// The following class structures has 7 hierarchy levels and 5 classes wide;
// At the end is a final class which uses multiple inheritance to combine all classes
/////////////////////////////////////////////////////////////////////////////////////////

CLASS(ClassMultipleBaseA)
CLASS_INHERIT(ClassMultiple1A, ClassMultipleBaseA)
CLASS_INHERIT(ClassMultiple2A, ClassMultiple1A)
CLASS_INHERIT(ClassMultiple3A, ClassMultiple2A)
CLASS_INHERIT(ClassMultiple4A, ClassMultiple3A)
CLASS_INHERIT(ClassMultiple5A, ClassMultiple4A)
CLASS_INHERIT(ClassMultiple6A, ClassMultiple5A)

CLASS(ClassMultipleBaseB)
CLASS_INHERIT(ClassMultiple1B, ClassMultipleBaseB)
CLASS_INHERIT(ClassMultiple2B, ClassMultiple1B)
CLASS_INHERIT(ClassMultiple3B, ClassMultiple2B)
CLASS_INHERIT(ClassMultiple4B, ClassMultiple3B)
CLASS_INHERIT(ClassMultiple5B, ClassMultiple4B)
CLASS_INHERIT(ClassMultiple6B, ClassMultiple5B)

CLASS(ClassMultipleBaseC)
CLASS_INHERIT(ClassMultiple1C, ClassMultipleBaseC)
CLASS_INHERIT(ClassMultiple2C, ClassMultiple1C)
CLASS_INHERIT(ClassMultiple3C, ClassMultiple2C)
CLASS_INHERIT(ClassMultiple4C, ClassMultiple3C)
CLASS_INHERIT(ClassMultiple5C, ClassMultiple4C)
CLASS_INHERIT(ClassMultiple6C, ClassMultiple5C)

CLASS(ClassMultipleBaseD)
CLASS_INHERIT(ClassMultiple1D, ClassMultipleBaseD)
CLASS_INHERIT(ClassMultiple2D, ClassMultiple1D)
CLASS_INHERIT(ClassMultiple3D, ClassMultiple2D)
CLASS_INHERIT(ClassMultiple4D, ClassMultiple3D)
CLASS_INHERIT(ClassMultiple5D, ClassMultiple4D)
CLASS_INHERIT(ClassMultiple6D, ClassMultiple5D)

CLASS(ClassMultipleBaseE)
CLASS_INHERIT(ClassMultiple1E, ClassMultipleBaseE)
CLASS_INHERIT(ClassMultiple2E, ClassMultiple1E)
CLASS_INHERIT(ClassMultiple3E, ClassMultiple2E)
CLASS_INHERIT(ClassMultiple4E, ClassMultiple3E)
CLASS_INHERIT(ClassMultiple5E, ClassMultiple4E)
CLASS_INHERIT(ClassMultiple6E, ClassMultiple5E)

CLASS_MULTI_INHERIT_5(FinalClass, ClassMultiple6A, ClassMultiple6B, ClassMultiple6C, ClassMultiple6D, ClassMultiple6E)

/////////////////////////////////////////////////////////////////////////////////////////
// The following class structures has 7 hierarchy levels and 2 classes wide;
// At the end is a final class which uses multiple inheritance to combine all classes
/////////////////////////////////////////////////////////////////////////////////////////

CLASS(ClassMulti1A)

CLASS(ClassMulti1B)
CLASS(ClassMulti2B)
CLASS(ClassMulti3B)
CLASS(ClassMulti4B)
CLASS(ClassMulti5B)
CLASS(ClassMulti6B)

CLASS_MULTI_INHERIT_2(ClassMulti2A, ClassMulti1A, ClassMulti1B)
CLASS_MULTI_INHERIT_2(ClassMulti3A, ClassMulti2A, ClassMulti2B)
CLASS_MULTI_INHERIT_2(ClassMulti4A, ClassMulti3A, ClassMulti3B)
CLASS_MULTI_INHERIT_2(ClassMulti5A, ClassMulti4A, ClassMulti4B)
CLASS_MULTI_INHERIT_2(ClassMulti6A, ClassMulti5A, ClassMulti5B)
CLASS_MULTI_INHERIT_2(ClassMulti7A, ClassMulti6A, ClassMulti6B)

#endif // __RTTR_TESTCLASSES_H__
