#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include "VulkanHelper.h"

namespace MiniEngine::Backend {
class VulkanBuffer
{
public:
    VulkanBuffer() = default;

    void Allocate();
    void Flush(void *data, size_t size);
    void Create(size_t memSize, VkBufferUsageFlags flags);

    VkBuffer getRawBuffer() const { return mBuffer; }

private:
    VkBuffer mBuffer;
    VkDevice mDevice;
    VkDeviceMemory mBufferMemory;
    VkPhysicalDeviceMemoryProperties mMemoryProperties;

    VulkanBuffer(VkDevice device, VkPhysicalDeviceMemoryProperties memProperties);

    friend class VulkanDriver;
};
} // namespace MiniEngine::Backend
#endif // VULKANBUFFER_H
