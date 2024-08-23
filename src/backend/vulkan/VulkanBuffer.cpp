#include "VulkanBuffer.h"

MiniEngine::Backend::VulkanBuffer::VulkanBuffer(VkDevice device,
                                                VkPhysicalDeviceMemoryProperties memProperties)
    : mDevice(device)
    , mMemoryProperties(memProperties)
{}

void MiniEngine::Backend::VulkanBuffer::create(VmaAllocator &allocator,
                                               size_t memSize,
                                               void *data,
                                               VkBufferUsageFlags flags)
{
    mSize = memSize;

    VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = memSize;
    bufferInfo.usage = flags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = nullptr;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    allocInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                               | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
                      | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VmaAllocation allocation;
    vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &mBuffer, &allocation, nullptr);
    vmaCopyMemoryToAllocation(allocator, data, allocation, 0, memSize);
}
