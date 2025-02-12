#pragma once

#include "EASTL/string.h"
#include "EASTL/memory.h"
// typedef uint32_t size_t;

namespace util
{
struct FileReader {
    const uint8_t* bytes;
    size_t cursor;

    uint8_t GetUInt8() { return static_cast<uint8_t>(bytes[cursor++]); }
    int8_t GetInt8() { return static_cast<int8_t>(bytes[cursor++]); }
    int16_t GetInt16() { return GetObj<int16_t>(); }
    uint16_t GetUInt16() { return GetObj<uint16_t>(); }
    int32_t GetInt32() { return GetObj<int32_t>(); }
    uint32_t GetUInt32() { return GetObj<uint32_t>(); }

    template<typename T>
    T GetObj()
    {
        T obj{};
        ReadObj(obj);
        return obj;
    }

    template<typename T>
    void ReadObj(T& obj)
    {
        __builtin_memcpy((void*)&obj, &bytes[cursor], sizeof(T));
        cursor += sizeof(T);
    }

    template<typename T>
    void ReadArr(T* arr, size_t numElements)
    {
        GetBytes(arr, numElements * sizeof(T));
    }

    void GetBytes(void* dest, size_t size)
    {
        __builtin_memcpy((void*)dest, &bytes[cursor], size);
        cursor += size;
    }

    void SkipBytes(size_t numBytes) { cursor += numBytes; }
};

} // end of namespace util