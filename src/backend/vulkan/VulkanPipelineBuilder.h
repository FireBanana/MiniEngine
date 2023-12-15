#pragma once

#include "VulkanHelper.h"
#include "RenderableComponent.h"

namespace MiniEngine
{
namespace Backend
{
	template<size_t B, size_t A>
	struct VertexStateData
	{
		VkPipelineVertexInputStateCreateInfo			 data;
		std::array<VkVertexInputBindingDescription, B>   vertexBindingDescriptions;
		std::array<VkVertexInputAttributeDescription, A> vertexAttributeDescription;
	};

	class VulkanPipelineBuilder
	{

	public:

		VulkanPipelineBuilder(VkDevice device, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties);

		VkPipelineLayout createEmptyPipelineLayout();
		VkPipelineLayout createDefaultPipelineLayout();

		template<size_t B, size_t A>
		VertexStateData<B, A> createCustomVertexInputState();

		VertexStateData<1, 2> createTriangleVertexInputState();
		VkPipelineInputAssemblyStateCreateInfo createDefaultInputAssemblyState();
		VkPipelineRasterizationStateCreateInfo createDefaultRasterState();
		VkPipelineColorBlendStateCreateInfo createDefaultPipelineColorBlendState();
		VkPipelineViewportStateCreateInfo createDefaultPipelineViewportState();
		VkPipelineMultisampleStateCreateInfo createDefaultPipelineMultisampleState();

		std::array<VkPipelineShaderStageCreateInfo, 2> createDefaultVertFragShaderStage(const char* vertPath, const char* fragPath);

		void instantiateTriangleBuffer();

		VkBuffer getDefaultTriangleBuffer() const { return mDefaultTriangleBuffer; }

	private:

		VkDevice mActiveDevice;
		VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
		VkBuffer mDefaultTriangleBuffer;

		Components::RenderableComponent mDefaultTriangleRenderableComponent;

		VkDescriptorSetLayout createEmptyDescriptorSetLayout();		
		VkDescriptorSetLayout createDefaultDescriptorSetLayout();

		VkBuffer createDefaultTriangleBuffer();
		VkDeviceMemory allocateDefaultTriangleBuffer(VkBuffer buffer);

	};
}
}