#include "VulkanDriver.h"
#include "VulkanImage.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

MiniEngine::Backend::VulkanImage::Builder::Builder(VulkanDriver *driver)
    : mDriver(driver)
    , mData(nullptr)
{}

MiniEngine::Backend::VulkanImage::Builder &MiniEngine::Backend::VulkanImage::Builder::setWidth(
    float width)
{
    mWidth = width;
    return *this;
}

MiniEngine::Backend::VulkanImage::Builder &MiniEngine::Backend::VulkanImage::Builder::setHeight(
    float height)
{
    mHeight = height;
    return *this;
}

MiniEngine::Backend::VulkanImage::Builder &MiniEngine::Backend::VulkanImage::Builder::setChannels(
    int channels)
{
    mChannels = channels;
    return *this;
}

MiniEngine::Backend::VulkanImage::Builder &MiniEngine::Backend::VulkanImage::Builder::setData(
    void *data)
{
    mData = data;
    return *this;
}

MiniEngine::Backend::VulkanImage::Builder &MiniEngine::Backend::VulkanImage::Builder::setFormat(
    VkFormat format)
{
    mFormat = format;
    return *this;
}

MiniEngine::Backend::VulkanImage::Builder &MiniEngine::Backend::VulkanImage::Builder::setUsageFlags(
    VkImageUsageFlags flags)
{
    mUsageFlags = flags;
    return *this;
}

MiniEngine::Backend::VulkanImage::Builder &MiniEngine::Backend::VulkanImage::Builder::setAspectFlags(
    VkImageAspectFlags flags)
{
    mAspectFlags = flags;
    return *this;
}

MiniEngine::Backend::VulkanImage::Builder &MiniEngine::Backend::VulkanImage::Builder::setDebugName(
    std::string name)
{
    mDebugName = name;
    return *this;
}

MiniEngine::Backend::VulkanImage MiniEngine::Backend::VulkanImage::Builder::build()
{
    VulkanImage image{};

    VkImageCreateInfo attachmentImageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    attachmentImageInfo.imageType = VK_IMAGE_TYPE_2D;
    attachmentImageInfo.format = mFormat;
    attachmentImageInfo.extent.width = mWidth;
    attachmentImageInfo.extent.height = mHeight;
    attachmentImageInfo.extent.depth = 1;
    attachmentImageInfo.arrayLayers = 1;
    attachmentImageInfo.mipLevels
        = 1; //static_cast<uint32_t>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1;
    attachmentImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    attachmentImageInfo.usage = mUsageFlags;
    attachmentImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImage attachmentImage;
    VkImageView attachmentImageView;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

    // Create staging buffer for image data
    if (mData) {
        VkBufferCreateInfo bufCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufCreateInfo.size = mWidth * mHeight * 32;
        bufCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                                | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkBuffer buf;
        VmaAllocation alloc;
        VmaAllocationInfo allocInfo;
        vmaCreateBuffer(
            mDriver->mMemoryAllocator, &bufCreateInfo, &allocCreateInfo, &buf, &alloc, &allocInfo);

        // Considering 32 bit pixels
        memcpy(allocInfo.pMappedData, mData, mWidth * mHeight * 4);

        image.mStagingBuffer = buf;
    }

    VmaAllocation allocation;
    auto r = vmaCreateImage(
        mDriver->mMemoryAllocator,
        &attachmentImageInfo,
        &allocInfo,
        &attachmentImage,
        &allocation,
        nullptr);

    VkImageViewCreateInfo colorImageViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    colorImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageViewInfo.format = mFormat;
    colorImageViewInfo.subresourceRange = {};
    colorImageViewInfo.subresourceRange.aspectMask = mAspectFlags;
    colorImageViewInfo.subresourceRange.baseMipLevel = 0;
    colorImageViewInfo.subresourceRange.levelCount = 1;
    colorImageViewInfo.subresourceRange.baseArrayLayer = 0;
    colorImageViewInfo.subresourceRange.layerCount = 1;
    colorImageViewInfo.image = attachmentImage;
    auto result = vkCreateImageView(
        mDriver->mActiveDevice, &colorImageViewInfo, nullptr, &attachmentImageView);

    if (r != VK_SUCCESS)
        MiniEngine::Logger::eprint("Image creation failed");

    if (result != VK_SUCCESS)
        MiniEngine::Logger::eprint("ImageView creation failed");

    create_debug_name(
        mDriver->mActiveDevice,
        mDebugName,
        VK_OBJECT_TYPE_IMAGE_VIEW,
        (uint64_t) attachmentImageView);

    image.mImage = attachmentImage;
    image.mImageView = attachmentImageView;
    image.mWidth = mWidth;
    image.mHeight = mHeight;

    return image;
}
