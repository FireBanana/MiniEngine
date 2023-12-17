#include "VulkanPipelineBuilder.h"
#include "GlslCompiler.h"
#include "RenderableComponent.h"

MiniEngine::Backend::VulkanPipelineBuilder::VulkanPipelineBuilder(VkDevice device, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties)
	: mActiveDevice(device), mPhysicalDeviceMemoryProperties(physicalDeviceMemoryProperties)
{
	mDefaultTriangleRenderableComponent = Components::RenderableComponent{};
	mDefaultTriangleRenderableComponent.buffer =
	{
		//vertex     color
		  0.5, -0.5,  0.0, 1.0, 0.0,
		  0.5,  0.5,  1.0, 1.0, 1.0,
		  -0.5, 0.5,  1.0, 1.0, 1.0
	};

	mDefaultTriangleRenderableComponent.indices =
	{
		0, 1, 2
	};
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

VkPipelineLayout MiniEngine::Backend::VulkanPipelineBuilder::createDefaultPipelineLayout()
{
	VkPipelineLayout pipelineLayout;
	return pipelineLayout;
}

template<size_t B, size_t A>
MiniEngine::Backend::VertexStateData<B, A> MiniEngine::Backend::VulkanPipelineBuilder::createCustomVertexInputState()
{
	// Todo: attributes will need to be changed
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

	std::array<VkVertexInputBindingDescription, B> vertexBindingDescriptions{};
	vertexBindingDescriptions[0] = {};
	vertexBindingDescriptions[0].binding = 0;
	vertexBindingDescriptions[0].stride = sizeof(float) * 5;
	vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::array<VkVertexInputAttributeDescription, A> vertexAttributeDescription{};
	vertexAttributeDescription[0] = {};
	vertexAttributeDescription[0].location = 0;
	vertexAttributeDescription[0].binding = vertexBindingDescriptions[0].binding;
	vertexAttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
	vertexAttributeDescription[0].offset = 0;

	vertexAttributeDescription[1] = {};
	vertexAttributeDescription[1].location = 1;
	vertexAttributeDescription[1].binding = vertexBindingDescriptions[0].binding;
	vertexAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexAttributeDescription[1].offset = sizeof(float) * 2;

	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescription.size());

	VertexStateData<B, A> state { 
		std::move(vertexInputInfo),
		std::move(vertexBindingDescriptions),
		std::move(vertexAttributeDescription)
	};

	return state;
}

MiniEngine::Backend::VertexStateData<1, 2> MiniEngine::Backend::VulkanPipelineBuilder::createTriangleVertexInputState()
{
	return createCustomVertexInputState<1, 2>();
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

VkDescriptorSetLayout MiniEngine::Backend::VulkanPipelineBuilder::createDefaultDescriptorSetLayout()
{
	return VkDescriptorSetLayout();
}

VkBuffer MiniEngine::Backend::VulkanPipelineBuilder::createDefaultTriangleBuffer()
{
	VkBuffer vertexBuffer;

	VkBufferCreateInfo bufferCreateInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferCreateInfo.size = mDefaultTriangleRenderableComponent.buffer.size() * sizeof(float);
	bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;

	VK_CHECK( vkCreateBuffer(mActiveDevice, &bufferCreateInfo, nullptr, &vertexBuffer) );

	return vertexBuffer;
}

VkDeviceMemory MiniEngine::Backend::VulkanPipelineBuilder::allocateDefaultTriangleBuffer(VkBuffer buffer)
{
	VkMemoryRequirements bufferMemRequirements;
	VkDeviceMemory memory;

	vkGetBufferMemoryRequirements(mActiveDevice, buffer, &bufferMemRequirements);

	for (auto i = 0; i < mPhysicalDeviceMemoryProperties.memoryTypeCount; ++i)
	{
		if ((bufferMemRequirements.memoryTypeBits & (1 << i)) && 
			(mPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		{
			VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			allocInfo.allocationSize = bufferMemRequirements.size;
			allocInfo.memoryTypeIndex = i;
		
			VK_CHECK( vkAllocateMemory(mActiveDevice, &allocInfo, nullptr, &memory) );
			return memory;
		}
	}

	Logger::eprint("Memory for default triangle buffer not allocated.");
	return memory;
}

void MiniEngine::Backend::VulkanPipelineBuilder::instantiateTriangleBuffer()
{
	mDefaultTriangleBuffer = createDefaultTriangleBuffer();
	auto bufferMemory = allocateDefaultTriangleBuffer(mDefaultTriangleBuffer);

	vkBindBufferMemory(mActiveDevice, mDefaultTriangleBuffer, bufferMemory, 0);

	void* bufferMemoryPointer;
	vkMapMemory(mActiveDevice, bufferMemory, 0, VK_WHOLE_SIZE, 0, &bufferMemoryPointer);
	memcpy(bufferMemoryPointer, mDefaultTriangleRenderableComponent.buffer.data(), mDefaultTriangleRenderableComponent.buffer.size() * sizeof(float));

	VkMappedMemoryRange flushRange{ VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
	flushRange.memory = bufferMemory;
	flushRange.offset = 0;
	flushRange.size = VK_WHOLE_SIZE;

	vkFlushMappedMemoryRanges(mActiveDevice, 1, &flushRange);

	vkUnmapMemory(mActiveDevice, bufferMemory);
}
