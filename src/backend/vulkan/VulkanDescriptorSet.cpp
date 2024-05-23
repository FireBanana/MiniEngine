#include "VulkanDescriptorSet.h"
#include "VulkanDriver.h"

MiniEngine::Backend::VulkanDescriptorSet::Builder::Builder(VulkanDriver *driver)
    : mDriver(driver)
{}

MiniEngine::Backend::VulkanDescriptorSet::Builder &
MiniEngine::Backend::VulkanDescriptorSet::Builder::setBinding(int binding)
{
    mBinding = binding;
    return *this;
}

MiniEngine::Backend::VulkanDescriptorSet::Builder &
MiniEngine::Backend::VulkanDescriptorSet::Builder::setCount(int count)
{
    mCount = count;
    return *this;
}

MiniEngine::Backend::VulkanDescriptorSet::Builder &
MiniEngine::Backend::VulkanDescriptorSet::Builder::setType(VkDescriptorType type)
{
    mType = type;
    return *this;
}

MiniEngine::Backend::VulkanDescriptorSet::Builder &
MiniEngine::Backend::VulkanDescriptorSet::Builder::setShaderStages(VkShaderStageFlags flags)
{
    mStageFlags = flags;
    return *this;
}

MiniEngine::Backend::VulkanDescriptorSet::Builder &
MiniEngine::Backend::VulkanDescriptorSet::Builder::setPool(VkDescriptorPool pool)
{
    mPool = pool;
    return *this;
}

MiniEngine::Backend::VulkanDescriptorSet MiniEngine::Backend::VulkanDescriptorSet::Builder::build()
{
    VulkanDescriptorSet set{};

    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};

    vkCreateDescriptorSetLayout(mDriver->mActiveDevice,
                                &descriptorLayoutInfo,
                                nullptr,
                                &set.mLayout);

    VkDescriptorSetLayoutBinding bindings{};
    bindings.binding = 0;
    bindings.descriptorCount = 1;
    bindings.stageFlags = mStageFlags;
    bindings.descriptorType = mType;

    descriptorLayoutInfo.bindingCount = 1;
    descriptorLayoutInfo.pBindings = &bindings;

    VkDescriptorSetAllocateInfo allocInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};

    allocInfo.descriptorPool = mPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &set.mLayout;

    vkAllocateDescriptorSets(mDriver->mActiveDevice, &allocInfo, &set.mDescriptorSet);

    set.mDriver = mDriver;
    set.mType = mType;

    // mSceneBlockBuffer = mDriver->createBuffer(sizeof(SceneBlock),
    //                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    // mSceneBlockBuffer.allocate();
    // mSceneBlockBuffer.flush(&mDefaultSceneBlock, sizeof(SceneBlock));

    return set;
}

void MiniEngine::Backend::VulkanDescriptorSet::loadData(VulkanBuffer &&buffer)
{
    mBuffers.push_back(buffer);
}

void MiniEngine::Backend::VulkanDescriptorSet::update()
{
    for (auto &buffer : mBuffers) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer.getRawBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = buffer.getSize();

        VkWriteDescriptorSet writeSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        writeSet.dstSet = mDescriptorSet;
        writeSet.dstBinding = 0;
        writeSet.dstArrayElement = 0;
        writeSet.descriptorCount = 1;
        writeSet.descriptorType = mType;
        writeSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(mDriver->mActiveDevice, 1, &writeSet, 0, nullptr);
    }
}
