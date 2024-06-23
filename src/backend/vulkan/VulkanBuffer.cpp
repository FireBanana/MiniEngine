#include "VulkanBuffer.h"

MiniEngine::Backend::VulkanBuffer::VulkanBuffer(VkDevice device,
                                                VkPhysicalDeviceMemoryProperties memProperties)
    : mDevice(device),
    mMemoryProperties(memProperties),
    mIsValid(false)
{}

void MiniEngine::Backend::VulkanBuffer::create(size_t memSize, VkBufferUsageFlags flags)
{
    VkBufferCreateInfo bufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferCreateInfo.size = memSize;
    bufferCreateInfo.usage = flags;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    mSize = memSize;

    vkCreateBuffer(mDevice, &bufferCreateInfo, nullptr, &mBuffer);
}

void MiniEngine::Backend::VulkanBuffer::allocate()
{
    VkMemoryRequirements bufferMemRequirements;
    vkGetBufferMemoryRequirements(mDevice, mBuffer, &bufferMemRequirements);

    for (auto i = 0; i < mMemoryProperties.memoryTypeCount; ++i) {
        if ((bufferMemRequirements.memoryTypeBits & (1 << i))
            && (mMemoryProperties.memoryTypes[i].propertyFlags
                & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
            allocInfo.allocationSize = bufferMemRequirements.size;
            allocInfo.memoryTypeIndex = i;

            vkAllocateMemory(mDevice, &allocInfo, nullptr, &mBufferMemory);
            mIsValid = true;
            return;
        }
    }

    Logger::eprint("Memory for buffer not allocated.");
}

void MiniEngine::Backend::VulkanBuffer::deallocate()
{
    vkFreeMemory(mDevice, mBufferMemory, nullptr);
}

void MiniEngine::Backend::VulkanBuffer::flush(void *data, size_t size)
{
    vkBindBufferMemory(mDevice, mBuffer, mBufferMemory, 0);

    void *bufferMemoryPointer;
    vkMapMemory(mDevice, mBufferMemory, 0, VK_WHOLE_SIZE, 0, &bufferMemoryPointer);
    memcpy(bufferMemoryPointer, data, size);

    VkMappedMemoryRange flushRange{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
    flushRange.memory = mBufferMemory;
    flushRange.offset = 0;
    flushRange.size = VK_WHOLE_SIZE;

    vkFlushMappedMemoryRanges(mDevice, 1, &flushRange);

    vkUnmapMemory(mDevice, mBufferMemory);
}
