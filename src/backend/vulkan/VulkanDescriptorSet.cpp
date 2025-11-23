#include "VulkanDescriptorSet.h"
#include "VulkanDriver.h"
#include <vulkan/vulkan_core.h>

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
MiniEngine::Backend::VulkanDescriptorSet::Builder::setTypeStructure(
    std::vector<VkDescriptorType> type)
{
    mTypeStructure = type;
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

MiniEngine::Backend::VulkanDescriptorSet::Builder &
MiniEngine::Backend::VulkanDescriptorSet::Builder::setDebugName(std::string &&name)
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

    const VkDescriptorBindingFlagsEXT flag
        = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT
          | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT
          | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT;

    std::vector<VkDescriptorBindingFlagsEXT> flags{};
    std::vector<VkDescriptorSetLayoutBinding> bindings{};

    for (auto i = 0; i < mTypeStructure.size(); ++i) {
        VkDescriptorSetLayoutBinding binding{};

        binding.binding = i;
        binding.descriptorCount = 1;
        binding.stageFlags = mStageFlags;
        binding.descriptorType = mTypeStructure[i];

        bindings.push_back(binding);

        flags.push_back(flag);
    }

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT binding_flags{};
    binding_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    binding_flags.bindingCount = mTypeStructure.size();
    binding_flags.pBindingFlags = flags.data();

    descriptorLayoutInfo.pNext = &binding_flags;

    descriptorLayoutInfo.bindingCount = mTypeStructure.size();
    descriptorLayoutInfo.pBindings = bindings.data();

    vkCreateDescriptorSetLayout(mDriver->mActiveDevice, &descriptorLayoutInfo, nullptr, &set.mLayout);

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
        debugNameInfo.objectHandle = (uint64_t) (set.mDescriptorSet);
        debugNameInfo.pObjectName = mDebugName.c_str();

        if (vkSetDebugUtilsObjectNameEXT(mDriver->mActiveDevice, &debugNameInfo) != VK_SUCCESS)
            MiniEngine::Logger::eprint("Error creating descriptor debug object");
    }
#endif

    set.mDriver = mDriver;

    for (auto i = 0; i < mTypeStructure.size(); ++i)
        set.mTypeStructure.push_back({mTypeStructure[i], -1});

    return set;
}

void MiniEngine::Backend::VulkanDescriptorSet::loadData(VulkanBuffer &&buffer, int structureIndex)
{
    if (mTypeStructure[structureIndex].first != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
        MiniEngine::Logger::eprint("Loading data into incorrect slot in descriptor set");
        return;
    }

    mBuffers.push_back(buffer);
    mTypeStructure[structureIndex].second = mBuffers.size() - 1;
}

void MiniEngine::Backend::VulkanDescriptorSet::loadData(VulkanImage image, int structureIndex)
{
    if (mTypeStructure[structureIndex].first != VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
        MiniEngine::Logger::eprint("Loading data into incorrect slot in descriptor set");
        return;
    }

    mImages.push_back(image);
    mTypeStructure[structureIndex].second = mImages.size() - 1;
}

void MiniEngine::Backend::VulkanDescriptorSet::update()
{
    mBufferInfos.clear();
    mImageInfos.clear();

    // Reserving for worst case scenario to prevent reallocation. could be improved
    mImageInfos.reserve(mTypeStructure.size());
    mBufferInfos.reserve(mTypeStructure.size());

    std::vector<VkWriteDescriptorSet> writeSetList{};

    for (auto i = 0; i < mTypeStructure.size(); ++i) {
        auto descriptorInfo = mTypeStructure[i];

        VkWriteDescriptorSet writeSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        writeSet.dstSet = mDescriptorSet;
        writeSet.dstBinding = descriptorInfo.second;
        writeSet.dstArrayElement = 0;
        writeSet.descriptorCount = 1;
        writeSet.descriptorType = descriptorInfo.first;

        if (descriptorInfo.first
            == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) { // TODO mtype should be individual to each descriptor
            auto &buffer = mBuffers[descriptorInfo.second];
            mBufferInfos.push_back({});
            auto &bufferInfo = mBufferInfos.back();
            bufferInfo.buffer = buffer.getRawBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = buffer.getSize();
            writeSet.pBufferInfo = &bufferInfo;
        } else if (descriptorInfo.first == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
            auto &image = mImages[descriptorInfo.second];
            mImageInfos.push_back({});
            auto &imageInfo = mImageInfos.back();
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = image.getImageView();
            writeSet.pImageInfo = &imageInfo;
        } else {
            MiniEngine::Logger::eprint("Descriptor type error during update");
        }

        writeSetList.push_back(writeSet);
    }

    // One for all descriptors?
    vkUpdateDescriptorSets(
        mDriver->mActiveDevice, writeSetList.size(), writeSetList.data(), 0, nullptr);
}
