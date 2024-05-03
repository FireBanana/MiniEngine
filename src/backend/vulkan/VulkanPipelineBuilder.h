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

        VkBuffer getDefaultTriangleBuffer() const { return mDefaultTriangleBuffer; }
        VkDescriptorSet getSceneBlockDescriptorSet() { return mSceneDescriptorSet; }

    private:
        VulkanDriver *mDriver;
        VkDevice mActiveDevice;
        VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
		VkBuffer mDefaultTriangleBuffer;
        VkBuffer mSceneBlockBuffer;
        SceneBlock mDefaultSceneBlock;

        Components::RenderableComponent mDefaultTriangleRenderableComponent;

        VkDescriptorSetLayout createEmptyDescriptorSetLayout();

        VkBuffer createDefaultTriangleBuffer();

        VkDescriptorSet mSceneDescriptorSet;
    };
}
}
#endif //MINIENGINE_VULKAN_PIPELNE_BUILDER
