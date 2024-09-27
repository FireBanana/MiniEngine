#include "VulkanBarrier.h"

MiniEngine::Backend::VulkanBarrier::Builder &MiniEngine::Backend::VulkanBarrier::Builder::setImage(
    VkImage image)
{
    mHasImage = true;
    mImage = image;
    return *this;
}

MiniEngine::Backend::VulkanBarrier::Builder &MiniEngine::Backend::VulkanBarrier::Builder::setBuffer(
    VkBuffer buffer)
{
    mHasBuffer = true;
    mBuffer = buffer;
    return *this;
}

MiniEngine::Backend::VulkanBarrier::Builder &
MiniEngine::Backend::VulkanBarrier::Builder::setCmdBuffer(VkCommandBuffer *cmd)
{
    mCmdBuffer = cmd;
    return *this;
}

MiniEngine::Backend::VulkanBarrier::Builder &
MiniEngine::Backend::VulkanBarrier::Builder::setSrcAccessMask(VkAccessFlags flags)
{
    mSrcAccessMask = flags;
    return *this;
}

MiniEngine::Backend::VulkanBarrier::Builder &
MiniEngine::Backend::VulkanBarrier::Builder::setDstAccessMask(VkAccessFlags flags)
{
    mDstAccessMask = flags;
    return *this;
}

MiniEngine::Backend::VulkanBarrier::Builder &
MiniEngine::Backend::VulkanBarrier::Builder::setAspectMask(VkImageAspectFlags flags)
{
    mAspectMask = flags;
    return *this;
}

MiniEngine::Backend::VulkanBarrier::Builder &
MiniEngine::Backend::VulkanBarrier::Builder::setOldLayout(VkImageLayout oldLayout)
{
    mOldLayout = oldLayout;
    return *this;
}

MiniEngine::Backend::VulkanBarrier::Builder &
MiniEngine::Backend::VulkanBarrier::Builder::setNewLayout(VkImageLayout newLayout)
{
    mNewLayout = newLayout;
    return *this;
}

MiniEngine::Backend::VulkanBarrier::Builder &
MiniEngine::Backend::VulkanBarrier::Builder::setSrcStageMask(VkPipelineStageFlags srcStageMask)
{
    mSrcStageMask = srcStageMask;
    return *this;
}

MiniEngine::Backend::VulkanBarrier::Builder &
MiniEngine::Backend::VulkanBarrier::Builder::setDstStageMask(VkPipelineStageFlags dstStageMask)
{
    mDstStageMask = dstStageMask;
    return *this;
}

MiniEngine::Backend::VulkanBarrier MiniEngine::Backend::VulkanBarrier::Builder::build()
{
    VulkanBarrier barrier{};

    VkImageMemoryBarrier imageMemBarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    imageMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemBarrier.srcAccessMask = mSrcAccessMask;
    imageMemBarrier.dstAccessMask = mDstAccessMask;
    imageMemBarrier.image = mImage;
    imageMemBarrier.subresourceRange.aspectMask = mAspectMask;
    imageMemBarrier.subresourceRange.levelCount = 1;
    imageMemBarrier.subresourceRange.layerCount = 1;
    imageMemBarrier.newLayout = mNewLayout;
    imageMemBarrier.oldLayout = mOldLayout;

    VkBufferMemoryBarrier bufferMemoryBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
    bufferMemoryBarrier.buffer = mBuffer;
    bufferMemoryBarrier.size = VK_WHOLE_SIZE;
    bufferMemoryBarrier.offset = 0;
    bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.srcAccessMask = mSrcAccessMask;
    bufferMemoryBarrier.dstAccessMask = mDstAccessMask;

    vkCmdPipelineBarrier(*mCmdBuffer,
                         mSrcStageMask,
                         mDstStageMask,
                         0,
                         0,
                         nullptr,
                         mHasBuffer ? 1 : 0,
                         mHasBuffer ? &bufferMemoryBarrier : nullptr,
                         mHasImage ? 1 : 0,
                         mHasImage ? &imageMemBarrier : nullptr);

    return barrier;
}
