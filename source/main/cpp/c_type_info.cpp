#include "ccore/c_debug.h"
#include "crttr/c_type_info.h"

#define RTTR_MAX_TYPE_COUNT          32767
#define RTTR_MAX_INHERIT_TYPES_COUNT 32

namespace ncore
{
    namespace RTTR
    {
        struct TypeInfoData
        {
            TypeInfoData()
                : globalIDCounter(0)
            {
            }

            static TypeInfoData &instance()
            {
                static TypeInfoData obj;
                return obj;
            }

            uint64_t hash(const char *name) const
            {
                // FNv-1a hash
                uint64_t hash = 14695981039346656037ULL;
                while (*name)
                {
                    hash ^= *name++;
                    hash *= 1099511628211ULL;
                }
                return hash;
            }

            bool findTypeId(const char *name, TypeInfo::TypeId &typeId) const
            {
                // hash the name, then search the hash array through the remap array
                // using binary search since the remap array is sorted by hash value.

                return 0;
            }

            static s8 sort_type_info(void const *inItemA, void const *inItemB, void *inUserData)
            {
                TypeInfoData *self = (TypeInfoData *)inUserData;
                s32 const    *a    = (s32 const *)inItemA;
                s32 const    *b    = (s32 const *)inItemB;

                return 0;
            }

            void sortTypeIdList()
            {
                if (lastIDCounter == globalIDCounter)
                    return;

                // Sort the range [lastIDCounter, globalIDCounter]
                // Insert the [lastIDCounter, globalIDCounter] range into the sorted list which
                // is [0, lastIDCounter], this is a lot more optimal than sorting the whole list.

                lastIDCounter = globalIDCounter;
            }

            TypeInfo::TypeId insertTypeId(const char *name, const TypeInfo &rawTypeInfo, const TypeInfo *baseClassList, int numBaseClasses)
            {
                if (globalIDCounter >= RTTR_MAX_TYPE_COUNT)
                {
                    return 0;
                }

                TypeInfo::TypeId newTypeId   = ++globalIDCounter;
                remap[newTypeId]             = newTypeId;
                nameList[newTypeId]          = name;
                hashList[newTypeId]          = hash(name);
                const TypeInfo::TypeId rawId = ((rawTypeInfo.getId() == 0) ? newTypeId : rawTypeInfo.getId());
                rawTypeList[newTypeId]       = rawId;
                const int row                = RTTR_MAX_INHERIT_TYPES_COUNT * rawId;
                int       index              = 0;
                // TODO remove double entries
                ASSERT(numBaseClasses < RTTR_MAX_INHERIT_TYPES_COUNT);
                for (int i = 0; i < numBaseClasses; ++i)
                {
                    inheritList[row + index] = baseClassList[i].getId();
                    ++index;
                }
                return newTypeId;
            }

            int         globalIDCounter;
            int         lastIDCounter;
            int         remap[RTTR_MAX_TYPE_COUNT];  // This map is sorted by the hash value of the name
            uint64_t    hashList[RTTR_MAX_TYPE_COUNT];
            const char *nameList[RTTR_MAX_TYPE_COUNT];

            TypeInfo::TypeId inheritList[RTTR_MAX_TYPE_COUNT * RTTR_MAX_INHERIT_TYPES_COUNT];
            TypeInfo::TypeId rawTypeList[RTTR_MAX_TYPE_COUNT];
        };

        /////////////////////////////////////////////////////////////////////////////////////////

        const char *TypeInfo::getName() const
        {
            if (!isValid())
                return "Invalid TypeInfo";
            TypeInfoData &data = TypeInfoData::instance();
            return data.nameList[m_id];
        }

        /////////////////////////////////////////////////////////////////////////////////////////

        TypeInfo TypeInfo::getRawType() const
        {
            if (!isValid())
                return TypeInfo();
            TypeInfoData &data = TypeInfoData::instance();
            return TypeInfo(data.rawTypeList[m_id]);
        }

        /////////////////////////////////////////////////////////////////////////////////////////

        bool TypeInfo::isTypeDerivedFrom(const TypeInfo &other) const
        {
            TypeInfoData          &data       = TypeInfoData::instance();
            const TypeInfo::TypeId thisRawId  = data.rawTypeList[m_id];
            const TypeInfo::TypeId otherRawId = data.rawTypeList[other.m_id];
            if (thisRawId == otherRawId)
                return true;

            const int row = RTTR_MAX_INHERIT_TYPES_COUNT * thisRawId;
            for (int i = 0; i < RTTR_MAX_INHERIT_TYPES_COUNT; ++i)
            {
                const TypeInfo::TypeId currId = data.inheritList[row + i];
                if (currId == otherRawId)
                    return true;
                if (currId == 0)  // invalid id
                    return false;
            }
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////

        namespace impl
        {
            TypeInfo registerOrGetType(const char *name, const TypeInfo &rawTypeInfo, const TypeInfo *baseClassList, int numBaseClasses)
            {
                TypeInfoData &data = TypeInfoData::instance();
                {
                    TypeInfo::TypeId typeId;
                    if (data.findTypeId(name, typeId))
                        return TypeInfo(typeId);
                }

                TypeInfo::TypeId newTypeId = data.insertTypeId(name, rawTypeInfo, baseClassList, numBaseClasses);
                return TypeInfo(newTypeId);
            }
        }  // end namespace impl
    }  // end namespace RTTR
}  // namespace ncore