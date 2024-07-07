#include "ccore/c_debug.h"
#include "crttr/c_type_info.h"

#include <map>
// #include <typeinfo>

using namespace std;

#define RTTR_MAX_TYPE_COUNT          32767
#define RTTR_MAX_INHERIT_TYPES_COUNT 50

namespace radixsort
{
    inline static void s_SwapPtr(uint64_t *&a, uint64_t *&b)
    {
        uint64_t *t = a;
        a           = b;
        b           = t;
    }

    struct item_t
    {
        uint64_t key;
        int      index;
    };

    uint64_t *RadixSort(uint64_t *pData, uint64_t *pTemp, int count, int imatrix[8][256])
    {
        uint64_t *pDst = nullptr;
        uint64_t *pSrc = nullptr;

        for (int i = 0; i < count; i++)
        {  // generate histograms
            uint64_t u = pData[i];
            for (int j = 0; j < 8; j++)
            {
                imatrix[j][(int)(u & 0xff)]++;
                u >>= 8;
            }
        }
        for (int j = 0; j < 8; j++)
        {  // convert to indices
            int n = 0;
            for (int i = 0; i < 256; i++)
            {
                uint64_t m    = imatrix[j][i];
                imatrix[j][i] = n;
                n += m;
            }
        }

        pDst = pTemp;  // radix sort
        pSrc = pData;
        for (int j = 0; j < 8; j++)
        {
            for (int i = 0; i < count; i++)
            {
                uint64_t u            = pSrc[i];
                uint64_t m            = (uint64_t)(u >> (j << 3)) & 0xff;
                pDst[imatrix[j][m]++] = u;
            }
            s_SwapPtr(pSrc, pDst);
        }
        return (pSrc);
    }
}  // namespace radixsort

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

    bool findTypeId(const char *name, RTTR::TypeInfo::TypeId &typeId) const
    {
        for (int i = 0; i < RTTR_MAX_TYPE_COUNT; ++i)
        {
            if (nameList[i] && strcmp(nameList[i], name) == 0)
                return rawTypeList[i];
        }
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

    RTTR::TypeInfo::TypeId insertTypeId(const char *name, const RTTR::TypeInfo &rawTypeInfo, const RTTR::TypeInfo *baseClassList, int numBaseClasses)
    {
        if (globalIDCounter >= RTTR_MAX_TYPE_COUNT)
        {
            return 0;
        }

        RTTR::TypeInfo::TypeId newTypeId   = ++globalIDCounter;
        remap[newTypeId]                   = newTypeId;
        nameList[newTypeId]                = name;
        hashList[newTypeId]                = hash(name);
        const RTTR::TypeInfo::TypeId rawId = ((rawTypeInfo.getId() == 0) ? newTypeId : rawTypeInfo.getId());
        rawTypeList[newTypeId]             = rawId;
        const int row                      = RTTR_MAX_INHERIT_TYPES_COUNT * rawId;
        int       index                    = 0;
        // TODO remove double entries
        ASSERT(numBaseClasses < RTTR_MAX_INHERIT_TYPES_COUNT);
        for (int i = 0; i < numBaseClasses; ++i)
        {
            inheritList[row + index] = baseClassList[i].getId();
            ++index;
        }
    }

    short       globalIDCounter;
    short       lastIDCounter;
    int         remap[RTTR_MAX_TYPE_COUNT];  // This map is sorted by the hash value of the name
    uint64_t    hashList[RTTR_MAX_TYPE_COUNT];
    const char *nameList[RTTR_MAX_TYPE_COUNT];

    RTTR::TypeInfo::TypeId inheritList[RTTR_MAX_TYPE_COUNT * RTTR_MAX_INHERIT_TYPES_COUNT];
    RTTR::TypeInfo::TypeId rawTypeList[RTTR_MAX_TYPE_COUNT];
};

namespace RTTR
{

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

            RTTR::TypeInfo::TypeId newTypeId = data.insertTypeId(name, rawTypeInfo, baseClassList, numBaseClasses);
            return TypeInfo(newTypeId);
        }

    }  // end namespace impl

}  // end namespace RTTR
