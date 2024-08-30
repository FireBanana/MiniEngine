#ifndef VULKAN_IMAGE
#define VULKAN_IMAGE

#include "VulkanHelper.h"

namespace MiniEngine::Backend
{
	class VulkanDriver;

	class VulkanImage 
	{
	public:
		class Builder 
		{
		public:
            Builder(VulkanDriver *driver);
            Builder(Builder &) = delete;
            Builder operator=(Builder&) = delete;
			Builder(Builder&&) = delete;
			Builder operator=(Builder&&) = delete;

			Builder& setWidth(float width);
			Builder& setHeight(float height);
			Builder& setChannels(int channels);
			Builder& setData(void* data);
			Builder& setFormat(VkFormat format);
			Builder& setUsageFlags(VkImageUsageFlags flags);
			Builder& setAspectFlags(VkImageAspectFlags flags);
			Builder& setDebugName(std::string name);
			VulkanImage build();

		private:
			VulkanDriver* mDriver;
			float mWidth;
			float mHeight;
			int mChannels;
			void* mData;
			VkFormat mFormat;
			VkImageUsageFlags mUsageFlags;
			VkImageAspectFlags mAspectFlags;
			std::string mDebugName;
		};

		VkImage getRawImage() const { return mImage; }
		VkDeviceMemory getMemory() const { return mMemory; }
		VkImageView getImageView() const { return mImageView; }

	private:
		VkImage mImage;
		VkDeviceMemory mMemory;
		VkImageView mImageView;

        friend class VulkanDriver;
    };
}

#endif
