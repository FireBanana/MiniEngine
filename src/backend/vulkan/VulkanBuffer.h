#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include "VulkanHelper.h"
#include "VulkanMemory.h"

namespace MiniEngine::Backend {
class VulkanBuffer
{
public:
    VulkanBuffer() = default;

    // Note: Calling vkFreeMemory in destructor causes error if the buffer
    // is moved, make additions to fix
    // void allocate();
    // void deallocate();
    // void flush(void *data, size_t size);

    VkBuffer getRawBuffer() const { return mBuffer; }
    size_t getSize() const { return mSize; }

private:
    size_t mSize;
    VkBuffer mBuffer;
    VkDevice mDevice;
    VkDeviceMemory mBufferMemory;
    VkPhysicalDeviceMemoryProperties mMemoryProperties;

    VulkanBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties);
    void create(VmaAllocator &allocator, size_t memSize, void *data, VkBufferUsageFlags flags);

    friend class VulkanDriver;
};
} // namespace MiniEngine::Backend
#endif // VULKANBUFFER_H
