#include "VulkanImage.h"
#include "VulkanDriver.h"

MiniEngine::Backend::VulkanImage::Builder::Builder(VulkanDriver* driver)
	:mDriver(driver)
{}

MiniEngine::Backend::VulkanImage::Builder& MiniEngine::Backend::VulkanImage::Builder::setWidth(float width)
{
	mWidth = width;
	return *this;
}

MiniEngine::Backend::VulkanImage::Builder& MiniEngine::Backend::VulkanImage::Builder::setHeight(float height)
{
	mHeight = height;
	return *this;

}

MiniEngine::Backend::VulkanImage::Builder& MiniEngine::Backend::VulkanImage::Builder::setChannels(int channels)
{
	mChannels = channels;
	return *this;
}

MiniEngine::Backend::VulkanImage::Builder& MiniEngine::Backend::VulkanImage::Builder::setData(void* data)
{
	mData = data;
	return *this;
}

MiniEngine::Backend::VulkanImage::Builder& MiniEngine::Backend::VulkanImage::Builder::setFormat(VkFormat format)
{
	mFormat = format;
	return *this;
}

MiniEngine::Backend::VulkanImage::Builder& MiniEngine::Backend::VulkanImage::Builder::setUsageFlags(VkImageUsageFlags flags)
{
	mUsageFlags = flags;
	return *this;
}

MiniEngine::Backend::VulkanImage::Builder& MiniEngine::Backend::VulkanImage::Builder::setAspectFlags(VkImageAspectFlags flags)
{
	mAspectFlags = flags;
	return *this;
}

MiniEngine::Backend::VulkanImage::Builder& MiniEngine::Backend::VulkanImage::Builder::setDebugName(std::string name)
{
	mDebugName = name;
	return *this;
}

MiniEngine::Backend::VulkanImage MiniEngine::Backend::VulkanImage::Builder::build()
{
	VulkanImage image{};

	VkImageCreateInfo attachmentImageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	attachmentImageInfo.imageType = VK_IMAGE_TYPE_2D;
	attachmentImageInfo.format = mFormat;
	attachmentImageInfo.extent.width = mWidth;
	attachmentImageInfo.extent.height = mHeight;
	attachmentImageInfo.extent.depth = 1;
	attachmentImageInfo.arrayLayers = 1;
	attachmentImageInfo.mipLevels = 1;
	attachmentImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	attachmentImageInfo.usage = mUsageFlags;

	VkMemoryAllocateInfo attachmentAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	VkMemoryRequirements attachmentMemReqs;
	VkImage attachmentImage;
	VkImageView attachmentImageView;
	VkDeviceMemory attachmentMemory;

	vkCreateImage(mDriver->mActiveDevice, &attachmentImageInfo, nullptr, &attachmentImage);
	vkGetImageMemoryRequirements(mDriver->mActiveDevice, attachmentImage, &attachmentMemReqs);

	// Get memory info

	uint32_t index = mDriver->getMemoryTypeIndex(&attachmentMemReqs);

	attachmentAllocateInfo.allocationSize = attachmentMemReqs.size;
	attachmentAllocateInfo.memoryTypeIndex = index;
	vkAllocateMemory(mDriver->mActiveDevice, &attachmentAllocateInfo, nullptr, &attachmentMemory);
	vkBindImageMemory(mDriver->mActiveDevice, attachmentImage, attachmentMemory, 0);

	VkImageViewCreateInfo colorImageViewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	colorImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	colorImageViewInfo.format = mFormat;
	colorImageViewInfo.subresourceRange = {};
	colorImageViewInfo.subresourceRange.aspectMask = mAspectFlags;
	colorImageViewInfo.subresourceRange.baseMipLevel = 0;
	colorImageViewInfo.subresourceRange.levelCount = 1;
	colorImageViewInfo.subresourceRange.baseArrayLayer = 0;
	colorImageViewInfo.subresourceRange.layerCount = 1;
	colorImageViewInfo.image = attachmentImage;
	vkCreateImageView(mDriver->mActiveDevice, &colorImageViewInfo, nullptr, &attachmentImageView);

#ifdef GRAPHICS_DEBUG

	VkDebugUtilsObjectNameInfoEXT imageNameInfo{};
	imageNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	imageNameInfo.pNext = NULL;
	imageNameInfo.objectType = VK_OBJECT_TYPE_IMAGE_VIEW;
	imageNameInfo.objectHandle = (uint64_t)attachmentImageView;
	imageNameInfo.pObjectName = mDebugName.c_str();

	vkSetDebugUtilsObjectNameEXT(mDriver->mActiveDevice, &imageNameInfo);

#endif

	image.mImage = attachmentImage;
	image.mImageView = attachmentImageView;
	image.mMemory = attachmentMemory;

	return image;
}
