#include "VulkanBuffer.h"
#include "VulkanDriver.h"

MiniEngine::Backend::VulkanBuffer::VulkanBuffer(VulkanDriver *driver,
                                                VkPhysicalDeviceMemoryProperties memProperties)
    : mDriver(driver)
    , mMemoryProperties(memProperties)
{}

void MiniEngine::Backend::VulkanBuffer::create(VmaAllocator &allocator,
                                               size_t memSize,
                                               void *data,
                                               VkBufferUsageFlags flags, 
                                               std::string&& debugName)
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

#ifdef GRAPHICS_DEBUG

    if (!debugName.empty()) {
        VkDebugUtilsObjectNameInfoEXT debugNameInfo{};
        debugNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        debugNameInfo.pNext = NULL;
        debugNameInfo.objectType = VK_OBJECT_TYPE_BUFFER;
        debugNameInfo.objectHandle = (uint64_t)mBuffer;
        debugNameInfo.pObjectName = debugName.c_str();

        vkSetDebugUtilsObjectNameEXT(mDriver->mActiveDevice, &debugNameInfo);
    }
#endif

    vmaCopyMemoryToAllocation(allocator, data, allocation, 0, memSize);
}
