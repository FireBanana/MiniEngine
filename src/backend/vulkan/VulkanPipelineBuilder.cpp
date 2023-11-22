#include "VulkanPipelineBuilder.h"
#include "GlslCompiler.h"

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

	return pipelineLayout;
}

VkPipelineVertexInputStateCreateInfo MiniEngine::Backend::VulkanPipelineBuilder::createEmptyPipelineVertexInputState()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo MiniEngine::Backend::VulkanPipelineBuilder::createDefaultInputAssemblyState()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	return inputAssemblyInfo;
}

VkPipelineRasterizationStateCreateInfo MiniEngine::Backend::VulkanPipelineBuilder::createDefaultRasterState()
{
	VkPipelineRasterizationStateCreateInfo rasterInfo{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterInfo.lineWidth = 1.0f;

	return rasterInfo;
}

VkPipelineColorBlendStateCreateInfo MiniEngine::Backend::VulkanPipelineBuilder::createDefaultPipelineColorBlendState()
{
	VkPipelineColorBlendAttachmentState colorBlendState{};
	colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendAttachmentState blendList[] = { colorBlendState };

	VkPipelineColorBlendStateCreateInfo colorBlendInfo{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	colorBlendInfo.attachmentCount = 1;
	colorBlendInfo.pAttachments = blendList;

	return colorBlendInfo;
}

VkPipelineViewportStateCreateInfo MiniEngine::Backend::VulkanPipelineBuilder::createDefaultPipelineViewportState()
{
	VkPipelineViewportStateCreateInfo viewportInfo{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewportInfo.viewportCount = 1;
	viewportInfo.scissorCount = 1;
	return viewportInfo;
}

VkPipelineMultisampleStateCreateInfo MiniEngine::Backend::VulkanPipelineBuilder::createDefaultPipelineMultisampleState()
{
	VkPipelineMultisampleStateCreateInfo multiSampleInfo{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multiSampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	return multiSampleInfo;
}

std::array<VkPipelineShaderStageCreateInfo, 2> MiniEngine::Backend::VulkanPipelineBuilder::createDefaultVertFragShaderStage(const char* vertPath, const char* fragPath)
{
	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module =
		MiniTools::GlslCompiler::loadShader(
			vertPath,
			VK_SHADER_STAGE_VERTEX_BIT,
			mActiveDevice);

	shaderStages[0].pName = "main";

	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module =
		MiniTools::GlslCompiler::loadShader(
			fragPath,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			mActiveDevice);

	shaderStages[1].pName = "main";

	return shaderStages;
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
