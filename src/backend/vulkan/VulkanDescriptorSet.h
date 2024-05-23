#ifndef VULKANDESCRIPTORSET_H
#define VULKANDESCRIPTORSET_H

#include "VulkanBuffer.h"
#include "VulkanHelper.h"

namespace MiniEngine::Backend {

class VulkanPipeline;
class VulkanDriver;

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
        Builder &setCount(int count);
        Builder &setType(VkDescriptorType type);
        Builder &setShaderStages(VkShaderStageFlags flags);
        Builder &setPool(VkDescriptorPool pool);
        VulkanDescriptorSet build();

    private:
        VulkanDriver *mDriver;
        VkDescriptorPool mPool;
        int mBinding;
        int mCount;
        VkDescriptorType mType;
        VkShaderStageFlags mStageFlags;
    };

    void loadData(VulkanBuffer &&buffer);
    void update();

    VkDescriptorSet *getDescriptorSet() { return &mDescriptorSet; }
    VkDescriptorSetLayout *getDescriptorSetLayout() { return &mLayout; }

private:
    std::vector<VulkanBuffer> mBuffers;
    VkDescriptorSet mDescriptorSet;
    VkDescriptorSetLayout mLayout;
    VkDescriptorType mType;
    VulkanDriver *mDriver;

    friend class VulkanDriver;
};
} // namespace MiniEngine::Backend
#endif // VULKANDESCRIPTORSET_H
