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
        VkPipelineLayout createDefaultPipelineLayout(VkPipelineLayout& oLayout);


		VertexStateData createCustomVertexInputState(Components::RenderableComponent* component);

		VertexStateData createTriangleVertexInputState();

        VkDescriptorSetLayout createSceneDescriptorLayout();
        VkDescriptorSetLayout createAttachmentDescriptorLayout();

		VkPipelineInputAssemblyStateCreateInfo createDefaultInputAssemblyState();
        VkPipelineRasterizationStateCreateInfo createDefaultRasterState();
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

		VkBuffer createDefaultTriangleBuffer();
		VkDeviceMemory allocateDefaultTriangleBuffer(VkBuffer buffer);

	};
}
}
