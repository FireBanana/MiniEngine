#ifndef MINIENGINE_VULKAN_PIPELNE_BUILDER
#define MINIENGINE_VULKAN_PIPELNE_BUILDER

#include "RenderableComponent.h"
#include "VulkanDriver.h"
#include "VulkanHelper.h"

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

    struct SceneBlock
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 cameraPosition;
    };

    class VulkanPipelineBuilder
    {

	public:

		VulkanPipelineBuilder(VkDevice device, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties);

		VkPipelineLayout createEmptyPipelineLayout();
        VkPipelineLayout createDefaultPipelineLayout(VkPipelineLayout &oLayout);

        VertexStateData createCustomVertexInputState(Components::RenderableComponent *component);

        VertexStateData createTriangleVertexInputState();

        VkDescriptorSetLayout createSceneDescriptorLayout();
        VkDescriptorSetLayout createAttachmentDescriptorLayout();

		VkPipelineInputAssemblyStateCreateInfo createDefaultInputAssemblyState();
        VkPipelineRasterizationStateCreateInfo createDefaultRasterState();
        VkPipelineViewportStateCreateInfo createDefaultPipelineViewportState();
		VkPipelineMultisampleStateCreateInfo createDefaultPipelineMultisampleState();

		std::array<VkPipelineShaderStageCreateInfo, 2> createDefaultVertFragShaderStage(const char* vertPath, const char* fragPath);

		void instantiateTriangleBuffer();
        void updateSceneDescriptorSetData();
        void updateAttachmentDescriptorSetData(
            std::array<VulkanDriver::ImageAttachmentData, 5> &attachments);

        VkBuffer getDefaultTriangleBuffer() const { return mDefaultTriangleBuffer; }
        std::array<VkDescriptorSet, 2> getDefaultDescriptorSets()
        {
            return {mSceneDescriptorSet, mAttachmentDescriptorSet};
        }

    private:

		VkDevice mActiveDevice;
		VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
		VkBuffer mDefaultTriangleBuffer;
        VkBuffer mSceneBlockBuffer;

        Components::RenderableComponent mDefaultTriangleRenderableComponent;

        VkDescriptorSetLayout createEmptyDescriptorSetLayout();

		VkBuffer createDefaultTriangleBuffer();
		VkDeviceMemory allocateDefaultTriangleBuffer(VkBuffer buffer);

        VkDescriptorSet mSceneDescriptorSet;
        VkDescriptorSet mAttachmentDescriptorSet;
    };
}
}
#endif //MINIENGINE_VULKAN_PIPELNE_BUILDER
