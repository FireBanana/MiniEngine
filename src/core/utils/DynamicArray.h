#pragma once

#ifndef MINIENGINE_DYNAMIC_ARRAY
#define MINIENGINE_DYNAMIC_ARRAY

#include <memory>

namespace MiniEngine {
namespace Utils {

template<typename T>
class DynamicArray
{
public:
    DynamicArray() {}

    DynamicArray(int size)
        : mArray(std::make_unique<T[]>(size))
        , mSize(size)
    {}

    ~DynamicArray() { mArray.reset(); }
    DynamicArray(DynamicArray &&) = default;
    DynamicArray &operator=(DynamicArray &&) = default;

    T *get() { return mArray.get(); }
    size_t getSize() const { return mSize; }

private:
    std::unique_ptr<T[]> mArray;
    size_t mSize;
};

} // namespace Utils
} // namespace MiniEngine

#endif
