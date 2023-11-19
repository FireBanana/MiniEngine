#pragma once

#include "VulkanHelper.h"

namespace MiniEngine
{
namespace Backend
{
	class VulkanPipelineBuilder
	{

	public:

		VulkanPipelineBuilder(VkDevice device);

		VkPipelineLayout createEmptyPipelineLayout();

	private:

		VkDevice mActiveDevice;

		VkDescriptorSetLayout createEmptyDescriptorSetLayout();

	};
}
}