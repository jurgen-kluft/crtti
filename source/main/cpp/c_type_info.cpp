#include "ccore/c_debug.h"
#include "crtti/c_type_info.h"

#define RTTR_MAX_TYPE_COUNT          32767
#define RTTR_MAX_INHERIT_TYPES_COUNT 32

namespace ncore
{
    namespace nrtti
    {
        struct type_info_data_t
        {
            type_info_data_t()
                : globalIDCounter(0)
            {
            }

            static type_info_data_t &instance()
            {
                static type_info_data_t obj;
                return obj;
            }

            uint64_t hash_name(const char *name) const
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

            bool find_type_id(const char *name, type_id_t &typeId) const
            {
                // hash the name, then search the hash array through the remap array
                // using binary search since the remap array is sorted by hash value.

                return 0;
            }

            static s8 sort_type_info(void const *inItemA, void const *inItemB, void *inUserData)
            {
                type_info_data_t *self = (type_info_data_t *)inUserData;
                s32 const    *a    = (s32 const *)inItemA;
                s32 const    *b    = (s32 const *)inItemB;

                return 0;
            }

            void sort_type_id_list()
            {
                if (lastIDCounter == globalIDCounter)
                    return;

                // Sort the range [lastIDCounter, globalIDCounter]
                // Insert the [lastIDCounter, globalIDCounter] range into the sorted list which
                // is [0, lastIDCounter], this is a lot more optimal than sorting the whole list.

                lastIDCounter = globalIDCounter;
            }

            type_id_t insert_type_id(const char *name, const type_info_t &rawTypeInfo, const type_info_t *baseClassList, int numBaseClasses)
            {
                if (globalIDCounter >= RTTR_MAX_TYPE_COUNT)
                {
                    return 0;
                }

                type_id_t newTypeId   = ++globalIDCounter;
                remap[newTypeId]             = newTypeId;
                nameList[newTypeId]          = name;
                hashList[newTypeId]          = hash_name(name);
                const type_id_t rawId = ((rawTypeInfo.getId() == 0) ? newTypeId : rawTypeInfo.getId());
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

            type_id_t inheritList[RTTR_MAX_TYPE_COUNT * RTTR_MAX_INHERIT_TYPES_COUNT];
            type_id_t rawTypeList[RTTR_MAX_TYPE_COUNT];
        };

        /////////////////////////////////////////////////////////////////////////////////////////

        const char *type_info_t::getName() const
        {
            if (!isValid())
                return "Invalid type_info_t";
            type_info_data_t &data = type_info_data_t::instance();
            return data.nameList[m_id];
        }

        /////////////////////////////////////////////////////////////////////////////////////////

        type_info_t type_info_t::getRawType() const
        {
            if (!isValid())
                return type_info_t();
            type_info_data_t &data = type_info_data_t::instance();
            return type_info_t(data.rawTypeList[m_id]);
        }

        /////////////////////////////////////////////////////////////////////////////////////////

        bool type_info_t::isTypeDerivedFrom(const type_info_t &other) const
        {
            type_info_data_t          &data       = type_info_data_t::instance();
            const type_id_t thisRawId  = data.rawTypeList[m_id];
            const type_id_t otherRawId = data.rawTypeList[other.m_id];
            if (thisRawId == otherRawId)
                return true;

            const int row = RTTR_MAX_INHERIT_TYPES_COUNT * thisRawId;
            for (int i = 0; i < RTTR_MAX_INHERIT_TYPES_COUNT; ++i)
            {
                const type_id_t currId = data.inheritList[row + i];
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
            type_info_t registerOrGetType(const char *name, const type_info_t &rawTypeInfo, const type_info_t *baseClassList, int numBaseClasses)
            {
                type_info_data_t &data = type_info_data_t::instance();
                {
                    type_id_t typeId;
                    if (data.find_type_id(name, typeId))
                        return type_info_t(typeId);
                }

                type_id_t newTypeId = data.insert_type_id(name, rawTypeInfo, baseClassList, numBaseClasses);
                return type_info_t(newTypeId);
            }
        }  // end namespace impl
    }  // end namespace RTTR
}  // namespace ncore