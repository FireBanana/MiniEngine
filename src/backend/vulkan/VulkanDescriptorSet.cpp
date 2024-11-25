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

MiniEngine::Backend::VulkanDescriptorSet::Builder&
MiniEngine::Backend::VulkanDescriptorSet::Builder::setDebugName(std::string&& name) 
{
    mDebugName = name;
    return *this;
}

MiniEngine::Backend::VulkanDescriptorSet MiniEngine::Backend::VulkanDescriptorSet::Builder::build()
{
    VulkanDescriptorSet set{};

    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    descriptorLayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

    const VkDescriptorBindingFlagsEXT flags
        = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT
          | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT
          | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT
          | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT;

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT binding_flags{};
    binding_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    binding_flags.bindingCount = 1;
    binding_flags.pBindingFlags = &flags;

    descriptorLayoutInfo.pNext = &binding_flags;

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

    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT variable_info{};
    uint32_t dCount = 1;
    variable_info.sType
        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    variable_info.descriptorSetCount = 1;
    variable_info.pDescriptorCounts = &dCount;
    allocInfo.pNext = &variable_info;

    vkAllocateDescriptorSets(mDriver->mActiveDevice, &allocInfo, &set.mDescriptorSet);

#ifdef GRAPHICS_DEBUG

    if (!mDebugName.empty()) {
        VkDebugUtilsObjectNameInfoEXT debugNameInfo{};
        debugNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        debugNameInfo.pNext = NULL;
        debugNameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
        debugNameInfo.objectHandle = (uint64_t)(set.mDescriptorSet);
        debugNameInfo.pObjectName = mDebugName.c_str();

        vkSetDebugUtilsObjectNameEXT(mDriver->mActiveDevice, &debugNameInfo);
    }
#endif

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
            auto &last = mBufferInfos.back();
            last.buffer = buffer.getRawBuffer();
            last.offset = 0;
            last.range = buffer.getSize();
            writeSet.pBufferInfo = &last;
        }
    } else if (mType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
        for (auto &image : mImages) {
            mImageInfos.push_back({});
            auto &last = mImageInfos.back();
            last.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            last.imageView = image->getImageView();
            writeSet.pImageInfo = &last;
        }
    } else {
        MiniEngine::Logger::eprint("Descriptor type error during update");
    }

    vkUpdateDescriptorSets(mDriver->mActiveDevice, 1, &writeSet, 0, nullptr);
}
