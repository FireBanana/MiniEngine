#ifndef MINIENGINE_VULKAN_PIPELINE
#define MINIENGINE_VULKAN_PIPELINE

#include "RenderableComponent.h"
#include "VulkanDescriptorSet.h"
#include "VulkanHelper.h"

namespace MiniEngine
{
namespace Backend
{
class VulkanDriver;

    class VulkanPipeline
    {
    public:
        class Builder
        {
        public:
            Builder(VulkanDriver *driver);
            Builder(Builder &) = delete;
            Builder operator=(Builder &) = delete;
            Builder(Builder &&) = delete;
            Builder operator=(Builder &&) = delete;

            Builder &addVertexAttributeState(int binding, std::vector<unsigned int> attr);
            Builder &addVertexBuffer(VulkanBuffer &&buffer);
            Builder &addDescriptorSet(VulkanDescriptorSet *set);
            Builder &addShaderState(const char *vert, const char *frag);
            Builder &setAttachmentCount(int count);
            Builder &setDepthState(bool depthTest, bool depthWrite);
            Builder &setRasterState(bool cullBack, bool clockWise);
            Builder &setDynamicState(std::vector<VkDynamicState> &&dynamicState);

            VulkanPipeline build();

        private:
            VkPipelineLayout mLayout;
            VulkanDriver *mDriver;
            int mAttachmentCount;
            std::vector<unsigned int> mVertexAttributes;
            std::vector<VulkanDescriptorSet *> mDescriptors;
            std::array<VkPipelineShaderStageCreateInfo, 2> mShaderStages;
            VkPipelineVertexInputStateCreateInfo mVertexInputStateCreateInfo;
            std::vector<VkVertexInputBindingDescription> mVertexBindingDescriptions;
            std::vector<VkVertexInputAttributeDescription> mVertexAttributeDescription;
            VkPipelineDepthStencilStateCreateInfo mDepthStencilStateCreateInfo;
            VkPipelineRasterizationStateCreateInfo mRasterStateCreateInfo;
            VkPipelineDynamicStateCreateInfo mDynamicStateCreateInfo;
            VulkanBuffer mVertexBuffer;
        };

    public:
        void bind(VkCommandBuffer buffer);        

    private:
        std::vector<VulkanDescriptorSet *> mDescriptors;
        VkPipeline mPipeline;
        VkPipelineLayout mPipelineLayout;
        VulkanBuffer mVertexBuffer;
        VulkanDriver *mDriver;

        void bindDescriptors(VkCommandBuffer cmd);
        void bindVertexBuffers(VkCommandBuffer cmd);

        friend class VulkanDriver;
    };
}
}
#endif //MINIENGINE_VULKAN_PIPELNE_BUILDER
