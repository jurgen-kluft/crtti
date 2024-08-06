#include "ccore/c_debug.h"
#include "ccore/c_qsort.h"
#include "ccore/c_binary_search.h"

#include "crtti/c_type_info.h"

#define RTTR_MAX_TYPE_COUNT          8192
#define RTTR_TMP_TYPE_COUNT          64
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

            struct name_hash_t
            {
                const char *m_name;
                u64         m_hash;
            };


            template <typename T>
            static inline s8 compare_values(T a, T b)
            {
                if (a < b)
                    return -1;
                if (a > b)
                    return 1;
                return 0;
            }

            static s8 compare_names(const char *nameA, const char *nameB)
            {
                while (*nameA && *nameB)
                {
                    s8 const result = compare_values(*nameA, *nameB);
                    if (result != 0)
                        return result;
                    ++nameA;
                    ++nameB;
                }
                return compare_values(*nameA, *nameB);
            }

            static s8 less_type_id(const void* _key, const void* _array, s64 _index, const void* _user_data)
            {
                type_info_data_t const* type_info_data = (type_info_data_t const*)_user_data;
                name_hash_t const* key = (name_hash_t const*)_key;
                s16 const remap = ((s16 const*)_array)[_index];
                s8 const hc = compare_values(key->m_hash, type_info_data->sortedHashList[remap]);
                if (hc == 0)
                {
                    s8 const nc = compare_names(key->m_name, type_info_data->sortedNameList);
                    return nc == -1;
                }
                return hc == -1;
            }

            static s8 equal_type_id(const void* _key, const void* _array, s64 _index, const void* _user_data)
            {
                type_info_data_t const* type_info_data = (type_info_data_t const*)_user_data;
                name_hash_t const* key = (name_hash_t const*)_key;
                s16 const remap = ((s16 const*)_array)[_index];
                s8 const hc = compare_values(key->m_hash, type_info_data->sortedHashList[remap]);
                if (hc == 0)
                {
                    s8 const nc = compare_names(key->m_name, type_info_data->sortedNameList);
                    return nc == 0;
                }
                return false;
            }

            bool find_type_id(const char *name, type_id_t &typeId) const
            {
                // hash the name, then search the hash array through the remap array
                // using binary search since the remap array is sorted by hash value.
                uint64_t hash = hash_name(name);
                name_hash_t key = { name, hash };

                g_BinarySearch(fullRemap, globalIDCounter, &key, )

                  return 0;
            }

            static s8 sort_type_info(void const *inItemA, void const *inItemB, void *inUserData)
            {
                type_info_data_t *self = (type_info_data_t *)inUserData;
                s16 const        *a    = (s16 const *)inItemA;
                s16 const        *b    = (s16 const *)inItemB;

                if (self->hashList[*a] < self->hashList[*b])
                    return -1;
                if (self->hashList[*a] > self->hashList[*b])
                    return 1;

                // compare the names
                const char *nameA = self->nameList[*a];
                const char *nameB = self->nameList[*b];
                return compare_names(nameA, nameB);
            }

            void sort_type_id_list()
            {
                if (lastIDCounter == globalIDCounter)
                    return;

                // TODO: Sort the range [lastIDCounter, globalIDCounter]
                // Merge the [lastIDCounter, globalIDCounter] range with the sorted range which
                // is [0, lastIDCounter], this is a lot more optimal than sorting the full range.
                g_qsort(remap, globalIDCounter, sizeof(remap[0]), sort_type_info, this);

                lastIDCounter = globalIDCounter;
            }

            type_id_t insert_type_id(const char *name, const type_info_t &rawTypeInfo, const type_info_t *baseClassList, int numBaseClasses)
            {
                if (globalIDCounter >= RTTR_MAX_TYPE_COUNT)
                {
                    return 0;
                }

                // Is the temporary remap array full? If so, sort it and merge it with the full remap array.
                if (globalIDCounter - lastIDCounter >= RTTR_TMP_TYPE_COUNT)
                {
                    sort_type_id_list();
                    // Merge the temporary remap array with the full remap array
                    // This is a lot more optimal than sorting the full range.
                }

                type_id_t newTypeId                        = globalIDCounter;
                tempRemap[globalIDCounter - lastIDCounter] = newTypeId;
                nameList[newTypeId]                        = name;
                hashList[newTypeId]                        = hash_name(name);
                const type_id_t rawId                      = ((rawTypeInfo.getId() == 0) ? newTypeId : rawTypeInfo.getId());
                rawTypeList[newTypeId]                     = rawId;
                const int row                              = RTTR_MAX_INHERIT_TYPES_COUNT * rawId;
                int       index                            = 0;
                // TODO remove double entries
                ASSERT(numBaseClasses < RTTR_MAX_INHERIT_TYPES_COUNT);
                for (int i = 0; i < numBaseClasses; ++i)
                {
                    inheritList[row + index] = baseClassList[i].getId();
                    ++index;
                }

                globalIDCounter++;
                return newTypeId;
            }

            int         globalIDCounter;
            int         lastIDCounter;
            s16         fullRemap[RTTR_MAX_TYPE_COUNT];  // This map is sorted by the hash value of the name
            s16         tempRemap[RTTR_TMP_TYPE_COUNT];  // This intermediate map is sorted by the hash value of the name
            uint64_t    sortedHashList[RTTR_MAX_TYPE_COUNT];
            const char *sortedNameList[RTTR_MAX_TYPE_COUNT];
            type_id_t   inheritList[RTTR_MAX_TYPE_COUNT * RTTR_MAX_INHERIT_TYPES_COUNT];
            type_id_t   rawTypeList[RTTR_MAX_TYPE_COUNT];
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
            type_info_data_t &data       = type_info_data_t::instance();
            const type_id_t   thisRawId  = data.rawTypeList[m_id];
            const type_id_t   otherRawId = data.rawTypeList[other.m_id];
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
    }  // namespace nrtti
}  // namespace ncore