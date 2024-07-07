#include "crttr/test_classes.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "crttr/c_rttr.h"
#include "cunittest/cunittest.h"

using namespace RTTR;

typedef std::vector<int>           VectorList;
typedef std::map<int, std::string> IntToStringMap;
typedef std::map<int, int>         IntToIntMap;

RTTR_DECLARE_STANDARD_META_TYPE_VARIANTS(std::string)
RTTR_DEFINE_STANDARD_META_TYPE_VARIANTS(std::string)

RTTR_DECLARE_STANDARD_META_TYPE_VARIANTS(VectorList)
RTTR_DEFINE_STANDARD_META_TYPE_VARIANTS(VectorList)

RTTR_DECLARE_STANDARD_META_TYPE_VARIANTS(IntToStringMap)
RTTR_DEFINE_STANDARD_META_TYPE_VARIANTS(IntToStringMap)

RTTR_DECLARE_STANDARD_META_TYPE_VARIANTS(IntToIntMap)
RTTR_DEFINE_STANDARD_META_TYPE_VARIANTS(IntToIntMap)

RTTR_DECLARE_META_TYPE(int***)
RTTR_DEFINE_META_TYPE(int***)

UNITTEST_SUITE_BEGIN(cmsg)
{
    UNITTEST_FIXTURE(url)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(TypeIdTests_BasicType)
        {
            using namespace RTTR;
            // check normal atomic types
            {
                int            intVar      = 23;
                const TypeInfo intTypeInfo = TypeInfo::get(intVar);
                CHECK_EQUAL(intTypeInfo.getName(), "int");
                CHECK_TRUE(intTypeInfo == TypeInfo::get<int>());

                bool           boolVar      = true;
                const TypeInfo boolTypeInfo = TypeInfo::get(boolVar);
                CHECK_EQUAL(boolTypeInfo.getName(), "bool");
                CHECK_TRUE(boolTypeInfo == TypeInfo::get<bool>());

                CHECK_TRUE(boolTypeInfo != intTypeInfo);

                int*** intPtr = NULL;
                CHECK_TRUE(TypeInfo::get<int***>() == TypeInfo::get(intPtr));
            }

            // check pointer types
            {
                int            intVar         = 23;
                int*           intPtrVar      = &intVar;
                const TypeInfo intPtrTypeInfo = TypeInfo::get(intPtrVar);
                CHECK_EQUAL(intPtrTypeInfo.getName(), "int*");
                CHECK_TRUE(intPtrTypeInfo == TypeInfo::get<int*>());

                bool           boolVar         = true;
                bool*          boolPtrVar      = &boolVar;
                const TypeInfo boolPtrTypeInfo = TypeInfo::get(boolPtrVar);
                CHECK_EQUAL(boolPtrTypeInfo.getName(), "bool*");
                CHECK_TRUE(boolPtrTypeInfo == TypeInfo::get<bool*>());

                CHECK_TRUE(boolPtrTypeInfo != intPtrTypeInfo);
            }

            // check that cv was removed
            {
                int       intVar      = 42;
                const int constIntVar = 42;
                CHECK_TRUE(TypeInfo::get(intVar) == TypeInfo::get(constIntVar));
                CHECK_TRUE(TypeInfo::get<int>() == TypeInfo::get(constIntVar));
                CHECK_TRUE(TypeInfo::get<int>() == TypeInfo::get<const int>());
                CHECK_TRUE(TypeInfo::get<int>() == TypeInfo::get<const int&>());

                CHECK_TRUE(TypeInfo::get<int*>() == TypeInfo::get(&intVar));
                CHECK_TRUE(TypeInfo::get<int*>() == TypeInfo::get<int* const>());
                CHECK_TRUE(TypeInfo::get<const int*>() == TypeInfo::get(&constIntVar));
                CHECK_TRUE(TypeInfo::get<const int*>() == TypeInfo::get<const int* const>());

                const int& intConstRef = intVar;
                CHECK_TRUE(TypeInfo::get<int>() == TypeInfo::get(intConstRef));
                int*** ptr = NULL;
                CHECK_TRUE(TypeInfo::get<int***>() == TypeInfo::get(ptr));
            }
        }

        UNITTEST_TEST(TypeIdTests_ComplexerTypes)
        {
            VectorList       myList;
            std::vector<int> myList2;
            CHECK_TRUE(TypeInfo::get<std::vector<int> >() == TypeInfo::get<VectorList>());
            CHECK_TRUE(TypeInfo::get<std::vector<int> >() == TypeInfo::get(myList));
            CHECK_TRUE(TypeInfo::get<VectorList>() == TypeInfo::get(myList));
            CHECK_TRUE(TypeInfo::get(myList) == TypeInfo::get(myList2));

            IntToStringMap             myMap;
            std::map<int, std::string> myMap2;

            CHECK_TRUE((TypeInfo::get<std::map<int, std::string> >() == TypeInfo::get<IntToStringMap>()));
            CHECK_TRUE((TypeInfo::get<std::map<int, std::string> >() == TypeInfo::get(myMap)));
            CHECK_TRUE((TypeInfo::get<IntToStringMap>() == TypeInfo::get(myMap)));
            CHECK_TRUE((TypeInfo::get(myMap) == TypeInfo::get(myMap2)));

            CHECK_TRUE((TypeInfo::get(myMap) != TypeInfo::get<std::map<int, int> >()));
        }

        UNITTEST_TEST(TypeIdTests_SingleClassInheritance)
        {
            {
                ClassSingle6A    instance6A;
                ClassSingleBase& baseSingle = instance6A;

                // down cast cast
                CHECK_TRUE(rttr_cast<ClassSingle6A*>(&baseSingle) != NULL);
                CHECK_TRUE(rttr_cast<ClassSingle3A*>(&baseSingle) != NULL);
                CHECK_TRUE(rttr_cast<ClassSingle6B*>(&baseSingle) == NULL);

                // up cast cast
                CHECK_TRUE(rttr_cast<ClassSingleBase*>(&instance6A) != NULL);
                CHECK_TRUE(rttr_cast<ClassSingle3A*>(&instance6A) != NULL);
                CHECK_TRUE(rttr_cast<ClassSingle1A*>(&instance6A) != NULL);
            }

            {
                ClassSingle6E    instance6E;
                ClassSingleBase& baseSingle = instance6E;

                // down cast cast
                CHECK_TRUE(rttr_cast<ClassSingle6A*>(&baseSingle) == NULL);
                CHECK_TRUE(rttr_cast<ClassSingle3E*>(&baseSingle) != NULL);
                CHECK_TRUE(rttr_cast<ClassSingle6E*>(&baseSingle) != NULL);

                // up cast cast
                CHECK_TRUE(rttr_cast<ClassSingleBase*>(&instance6E) != NULL);
                CHECK_TRUE(rttr_cast<ClassSingle3E*>(&instance6E) != NULL);
                CHECK_TRUE(rttr_cast<ClassSingle1E*>(&instance6E) != NULL);
                CHECK_TRUE(rttr_cast<ClassSingle6E*>(&instance6E) != NULL);
            }
        }

        UNITTEST_TEST(TypeIdTests_MultipleClassInheritance)
        {
            {
                FinalClass          final;
                ClassMultipleBaseA& baseMultiA = final;
                ClassMultipleBaseB& baseMultiB = final;
                ClassMultipleBaseC& baseMultiC = final;
                ClassMultipleBaseD& baseMultiD = final;
                ClassMultipleBaseE& baseMultiE = final;

                // down cast cast
                CHECK_TRUE(rttr_cast<FinalClass*>(&baseMultiA) != NULL);
                CHECK_TRUE(rttr_cast<FinalClass*>(&baseMultiB) != NULL);
                CHECK_TRUE(rttr_cast<FinalClass*>(&baseMultiC) != NULL);
                CHECK_TRUE(rttr_cast<FinalClass*>(&baseMultiD) != NULL);
                CHECK_TRUE(rttr_cast<FinalClass*>(&baseMultiE) != NULL);

                // up cast cast
                CHECK_TRUE(rttr_cast<ClassMultipleBaseA*>(&final) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultipleBaseB*>(&final) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultipleBaseC*>(&final) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultipleBaseD*>(&final) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultipleBaseE*>(&final) != NULL);

                // down cast cast to the middle
                CHECK_TRUE(rttr_cast<ClassMultiple3A*>(&baseMultiA) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultiple3B*>(&baseMultiB) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultiple3C*>(&baseMultiC) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultiple3D*>(&baseMultiD) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultiple3E*>(&baseMultiE) != NULL);
            }

            {
                FinalClass      final;
                ClassMultiple6A classMulti6A;
                ClassMultiple3B classMulti3B;
                ClassMultiple1E classMulti1E;

                // check for invalid casts
                CHECK_TRUE(rttr_cast<FinalClass*>(&classMulti6A) == NULL);

                CHECK_TRUE(rttr_cast<ClassMultiple4B*>(&classMulti3B) == NULL);

                CHECK_TRUE(rttr_cast<ClassMultiple2E*>(&classMulti1E) == NULL);
                CHECK_TRUE(rttr_cast<ClassMultiple6E*>(&classMulti1E) == NULL);

                // check for valid casts
                CHECK_TRUE(rttr_cast<ClassMultiple5A*>(&classMulti6A) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultipleBaseA*>(&classMulti6A) != NULL);

                CHECK_TRUE(rttr_cast<ClassMultiple2B*>(&classMulti3B) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultiple1B*>(&classMulti3B) != NULL);

                CHECK_TRUE(rttr_cast<ClassMultipleBaseE*>(&classMulti1E) != NULL);
                CHECK_TRUE(rttr_cast<ClassMultiple1E*>(&classMulti1E) != NULL);
            }
        }

        UNITTEST_TEST(TypeIdTests_TypeIdAndClassInheritance)
        {
            ClassSingle6A    instance6A;
            ClassSingleBase& baseSingle = instance6A;

            CHECK_TRUE(TypeInfo::get<ClassSingle6A>() == TypeInfo::get(baseSingle));
            CHECK_TRUE(TypeInfo::get<ClassSingleBase*>() == TypeInfo::get(&baseSingle));

            ClassSingle3A instance3A;
            CHECK_TRUE(TypeInfo::get<ClassSingle3A>() == TypeInfo::get(instance3A));
            CHECK_TRUE(TypeInfo::get<ClassSingle6A>() != TypeInfo::get(instance3A));
        }
    }
}
UNITTEST_SUITE_END
