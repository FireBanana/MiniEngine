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

    std::vector<VkDescriptorSetLayoutBinding> bindings{};

    for (auto i = 0; i < mCount; ++i) {
        VkDescriptorSetLayoutBinding binding{};

        binding.binding = i;
        binding.descriptorCount = 1;
        binding.stageFlags = mStageFlags;
        binding.descriptorType = mType;

        bindings.push_back(binding);
    }

    descriptorLayoutInfo.bindingCount = mCount;
    descriptorLayoutInfo.pBindings = bindings.data();

    vkCreateDescriptorSetLayout(mDriver->mActiveDevice,
                                &descriptorLayoutInfo,
                                nullptr,
                                &set.mLayout);

    VkDescriptorSetAllocateInfo allocInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};

    allocInfo.descriptorPool = mPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &set.mLayout;

    vkAllocateDescriptorSets(mDriver->mActiveDevice, &allocInfo, &set.mDescriptorSet);

    set.mDriver = mDriver;
    set.mType = mType;

    return set;
}

void MiniEngine::Backend::VulkanDescriptorSet::loadData(VulkanBuffer &&buffer)
{
    mBuffers.push_back(buffer);
}

void MiniEngine::Backend::VulkanDescriptorSet::loadData(VulkanImage *images)
{
    mImages.push_back(images);
}

void MiniEngine::Backend::VulkanDescriptorSet::update()
{
    VkWriteDescriptorSet writeSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    writeSet.dstSet = mDescriptorSet;
    writeSet.dstBinding = 0;
    writeSet.dstArrayElement = 0;
    writeSet.descriptorCount = 1;
    writeSet.descriptorType = mType;

    if (mType
        == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) { // TODO mtype should be individual to each descriptor
        for (auto &buffer : mBuffers) {
            mBufferInfos.push_back({});
            auto last = mBufferInfos.back();
            last.buffer = buffer.getRawBuffer();
            last.offset = 0;
            last.range = buffer.getSize();
            writeSet.pBufferInfo = &last;
        }
    } else if (mType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
        for (auto &image : mImages) {
            mImageInfos.push_back({});
            auto last = mImageInfos.back();
            last.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            last.imageView = image->getImageView();
            writeSet.pImageInfo = &last;
        }
    } else {
        MiniEngine::Logger::eprint("Descriptor type error during update");
    }

    vkUpdateDescriptorSets(mDriver->mActiveDevice, 1, &writeSet, 0, nullptr);
}
