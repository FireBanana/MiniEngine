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
    DynamicArray(int size)
        : mArray(std::make_unique<T[]>(size))
    {}

    ~DynamicArray() { mArray.reset(); }
    DynamicArray(DynamicArray &&) = default;
    DynamicArray &operator=(DynamicArray &&) = default;

    T *get() { return mArray.get(); }

private:
    std::unique_ptr<T> mArray;
};

} // namespace Utils
} // namespace MiniEngine

#endif
