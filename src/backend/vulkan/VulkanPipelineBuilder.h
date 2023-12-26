#pragma once

#include "VulkanHelper.h"
#include "RenderableComponent.h"

namespace MiniEngine
{
namespace Backend
{
	struct VertexStateData
	{
		VkPipelineVertexInputStateCreateInfo		   data;
		std::vector<VkVertexInputBindingDescription>   vertexBindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescription;
	};

	class VulkanPipelineBuilder
	{

	public:

		VulkanPipelineBuilder(VkDevice device, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties);

		VkPipelineLayout createEmptyPipelineLayout();
		VkPipelineLayout createDefaultPipelineLayout();


		VertexStateData createCustomVertexInputState(Components::RenderableComponent* component);

		VertexStateData createTriangleVertexInputState();

		void createDefaultDescriptorPool();
		void allocateDefaultDescriptorSet();

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

		VkDescriptorPool mDefaultDescriptorPool;
		VkDescriptorSetLayout mDefaultDescriptorLayout;

		Components::RenderableComponent mDefaultTriangleRenderableComponent;

		VkDescriptorSetLayout createEmptyDescriptorSetLayout();		
		VkDescriptorSetLayout createDefaultDescriptorSetLayout();

		VkBuffer createDefaultTriangleBuffer();
		VkDeviceMemory allocateDefaultTriangleBuffer(VkBuffer buffer);

	};
}
}