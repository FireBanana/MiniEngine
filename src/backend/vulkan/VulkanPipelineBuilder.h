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
        VulkanPipelineBuilder(VulkanDriver *driver);

        VkPipelineLayout createEmptyPipelineLayout();
        VkPipelineLayout createDefaultPipelineLayout(VkPipelineLayout &oLayout);

        VertexStateData createCustomVertexInputState(Components::RenderableComponent *component);

        VertexStateData createTriangleVertexInputState();

        VkDescriptorSetLayout createSceneDescriptorLayout();

        VkPipelineInputAssemblyStateCreateInfo createDefaultInputAssemblyState();
        VkPipelineRasterizationStateCreateInfo createDefaultRasterState();
        VkPipelineViewportStateCreateInfo createDefaultPipelineViewportState();
		VkPipelineMultisampleStateCreateInfo createDefaultPipelineMultisampleState();

		std::array<VkPipelineShaderStageCreateInfo, 2> createDefaultVertFragShaderStage(const char* vertPath, const char* fragPath);

		void instantiateTriangleBuffer();
        void updateSceneDescriptorSetData();

        VkDescriptorSet getSceneBlockDescriptorSet() { return mSceneDescriptorSet; }

        VulkanBuffer getDefaultTriangleBuffer() const { return mTriangleBuffer; }

    private:
        VulkanDriver *mDriver;
        VkDevice mActiveDevice;
        VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
        SceneBlock mDefaultSceneBlock;
        VulkanBuffer mSceneBlockBuffer;
        VulkanBuffer mTriangleBuffer;

        Components::RenderableComponent mDefaultTriangleRenderableComponent;

        VkDescriptorSetLayout createEmptyDescriptorSetLayout();
        VkDescriptorSet mSceneDescriptorSet;
    };
}
}
#endif //MINIENGINE_VULKAN_PIPELNE_BUILDER
