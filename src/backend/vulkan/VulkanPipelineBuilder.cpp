#include "VulkanPipelineBuilder.h"

MiniEngine::Backend::VulkanPipelineBuilder::VulkanPipelineBuilder(VkDevice device)
	: mActiveDevice(device)
{

}

VkPipelineLayout MiniEngine::Backend::VulkanPipelineBuilder::createEmptyPipelineLayout()
{
	VkPipelineLayout pipelineLayout;
	auto descriptorLayout = createEmptyDescriptorSetLayout();

	VkPipelineLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &descriptorLayout;
	VK_CHECK(vkCreatePipelineLayout(mActiveDevice, &layoutInfo, nullptr, &pipelineLayout));
}

VkDescriptorSetLayout MiniEngine::Backend::VulkanPipelineBuilder::createEmptyDescriptorSetLayout()
{
	VkDescriptorSetLayout defaultLayout;
	VkDescriptorSetLayoutCreateInfo descriptorSetInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	descriptorSetInfo.bindingCount = 0;
	descriptorSetInfo.pBindings = nullptr;

	VK_CHECK(vkCreateDescriptorSetLayout(mActiveDevice, &descriptorSetInfo, nullptr, &defaultLayout));

	return defaultLayout;
}
