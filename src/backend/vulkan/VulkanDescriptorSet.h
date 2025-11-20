#ifndef VULKANDESCRIPTORSET_H
#define VULKANDESCRIPTORSET_H

#include "VulkanBuffer.h"
#include "VulkanHelper.h"

namespace MiniEngine::Backend {

class VulkanPipeline;
class VulkanDriver;
class VulkanImage;

class VulkanDescriptorSet
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

        Builder &setBinding(int binding);
        Builder &setTypeStructure(std::vector<VkDescriptorType> type);
        Builder &setShaderStages(VkShaderStageFlags flags);
        Builder &setPool(VkDescriptorPool pool); // autoselect based on type?
        Builder &setDebugName(std::string &&name);
        VulkanDescriptorSet build();

    private:
        VulkanDriver *mDriver;
        VkDescriptorPool mPool;
        int mBinding;
        std::string mDebugName;
        std::vector<VkDescriptorType> mTypeStructure;
        VkShaderStageFlags mStageFlags;
    };

    void loadData(VulkanBuffer &&buffer, int structureIndex);
    void loadData(VulkanImage image, int structureIndex);
    void update();

    VkDescriptorSet *getDescriptorSet() { return &mDescriptorSet; }
    VkDescriptorSetLayout *getDescriptorSetLayout() { return &mLayout; }

private:
    std::vector<VulkanBuffer> mBuffers;
    std::vector<VulkanImage> mImages;
    std::vector<VkDescriptorBufferInfo> mBufferInfos;
    std::vector<VkDescriptorImageInfo> mImageInfos;
    VkDescriptorSet mDescriptorSet;
    VkDescriptorSetLayout mLayout;
    std::vector<std::pair<VkDescriptorType, int>>
        mTypeStructure; // The int represents the index into the specific array
    VulkanDriver *mDriver;

    friend class VulkanDriver;
};
} // namespace MiniEngine::Backend
#endif // VULKANDESCRIPTORSET_H
