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
		VkPipelineVertexInputStateCreateInfo createEmptyPipelineVertexInputState();
		VkPipelineInputAssemblyStateCreateInfo createDefaultInputAssemblyState();
		VkPipelineRasterizationStateCreateInfo createDefaultRasterState();
		VkPipelineColorBlendStateCreateInfo createDefaultPipelineColorBlendState();
		VkPipelineViewportStateCreateInfo createDefaultPipelineViewportState();
		VkPipelineMultisampleStateCreateInfo createDefaultPipelineMultisampleState();

		std::array<VkPipelineShaderStageCreateInfo, 2> createDefaultVertFragShaderStage(const char* vertPath, const char* fragPath);

	private:

		VkDevice mActiveDevice;

		VkDescriptorSetLayout createEmptyDescriptorSetLayout();		

	};
}
}