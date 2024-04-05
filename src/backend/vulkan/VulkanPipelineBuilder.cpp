#include "VulkanPipelineBuilder.h"
#include "GlslCompiler.h"
#include "RenderableComponent.h"
#include "EnumExtension.h"
#include "VulkanDriver.h"

#include <glm/gtc/matrix_transform.hpp>
#include <numeric>

MiniEngine::Backend::VulkanPipelineBuilder::VulkanPipelineBuilder(VulkanDriver *driver)
    : mDriver(driver)
    , mActiveDevice(driver->mActiveDevice)
    , mDefaultSceneBlock()
{
    mDefaultSceneBlock.cameraPosition = glm::vec3(0, 0, -5);
    mDefaultSceneBlock.projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    mDefaultSceneBlock.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec3(1.0f, 0.0f, 0.0f),
                                          glm::vec3(0.0f, -1.0f, 0.0f));

    mDefaultTriangleRenderableComponent = Components::RenderableComponent{};
    mDefaultTriangleRenderableComponent.buffer =
    {
        //vertex     color
          0.5, -0.5,  1.0, 0.0, 0.0,
          0.5,  0.5,  1.0, 1.0, 0.0,
          -0.5, 0.5,  0.0, 0.0, 1.0
    };

    mDefaultTriangleRenderableComponent.indices = { 0, 1, 2 };

    mDefaultTriangleRenderableComponent.attributes.push_back(2);
    mDefaultTriangleRenderableComponent.attributes.push_back(3);
}

VkPipelineLayout MiniEngine::Backend::VulkanPipelineBuilder::createEmptyPipelineLayout()
{
	VkPipelineLayout pipelineLayout;
	auto descriptorLayout = createEmptyDescriptorSetLayout();

	VkPipelineLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &descriptorLayout;
    vkCreatePipelineLayout(mActiveDevice, &layoutInfo, nullptr, &pipelineLayout);

    return pipelineLayout;
}

VkPipelineLayout MiniEngine::Backend::VulkanPipelineBuilder::createDefaultPipelineLayout(
    VkPipelineLayout &oLayout)
{
    auto sceneLayout = createSceneDescriptorLayout();
    auto attachmentLayout = createAttachmentDescriptorLayout();

    VkDescriptorSetLayout arr[] = { sceneLayout, attachmentLayout };

    VkPipelineLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    layoutInfo.setLayoutCount = 2;
    layoutInfo.pSetLayouts = arr;
    vkCreatePipelineLayout(mActiveDevice, &layoutInfo, nullptr, &oLayout);

    return oLayout;
}

MiniEngine::Backend::VertexStateData MiniEngine::Backend::VulkanPipelineBuilder::createCustomVertexInputState(Components::RenderableComponent* component)
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

	std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions{1};
	vertexBindingDescriptions[0] = {};
	vertexBindingDescriptions[0].binding = 0;
	vertexBindingDescriptions[0].stride = std::accumulate(component->attributes.begin(), component->attributes.end(), 0) * sizeof(float);
	vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescription{};
	int offset = 0;

	for (auto i = 0; i < component->attributes.size(); ++i)
	{
		VkVertexInputAttributeDescription value{};
		value.location = i;
		value.binding = vertexBindingDescriptions[0].binding;
		value.format = component->attributes[i] == 3 ? VK_FORMAT_R32G32B32_SFLOAT : VK_FORMAT_R32G32_SFLOAT; // either 3 or 2
		value.offset = offset * sizeof(float);

		vertexAttributeDescription.push_back(value);

		offset += component->attributes[i];
	}

	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescription.size());

	VertexStateData state { 
		std::move(vertexInputInfo),
		std::move(vertexBindingDescriptions),
		std::move(vertexAttributeDescription)
	};

	return state;
}

MiniEngine::Backend::VertexStateData MiniEngine::Backend::VulkanPipelineBuilder::createTriangleVertexInputState()
{
	return createCustomVertexInputState(&mDefaultTriangleRenderableComponent);
}

VkDescriptorSetLayout MiniEngine::Backend::VulkanPipelineBuilder::createSceneDescriptorLayout()
{
    VkDescriptorPoolSize uniformScenePoolSize{};

    // view, projection, and camera position
    uniformScenePoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformScenePoolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPoolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };

    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.maxSets = 1;
    descriptorPoolInfo.pPoolSizes = &uniformScenePoolSize;

	VkDescriptorSetLayoutBinding bindings{};
	bindings.binding = 0;
	bindings.descriptorCount = 1;
    bindings.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	descriptorLayoutInfo.bindingCount = 1;
	descriptorLayoutInfo.pBindings = &bindings;

    VkDescriptorPool pool;
    VkDescriptorSetLayout layout;
    vkCreateDescriptorPool(mActiveDevice, &descriptorPoolInfo, nullptr, &pool);
    vkCreateDescriptorSetLayout(mActiveDevice, &descriptorLayoutInfo, nullptr, &layout);

    VkDescriptorSetAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };

    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    vkAllocateDescriptorSets(mActiveDevice, &allocInfo, &mSceneDescriptorSet);

    VkBufferCreateInfo bufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferCreateInfo.size = sizeof(SceneBlock);
    bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    vkCreateBuffer(mActiveDevice, &bufferCreateInfo, nullptr, &mSceneBlockBuffer);
    auto bufferMemory = mDriver->allocateBuffer(mSceneBlockBuffer);
    mDriver->pushBufferMemory(mSceneBlockBuffer,
                              bufferMemory,
                              &mDefaultSceneBlock,
                              sizeof(SceneBlock));

    return layout;
}

VkDescriptorSetLayout MiniEngine::Backend::VulkanPipelineBuilder::createAttachmentDescriptorLayout()
{
    VkDescriptorPoolSize imageAttachmentPoolSize{};
    // Size sans depth and swapchain
    constexpr auto attachmentSize = EnumExtension::elementCount<VulkanDriver::ImageAttachmentType>()
                                    - 2;

    imageAttachmentPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    imageAttachmentPoolSize.descriptorCount = attachmentSize;

    VkDescriptorPoolCreateInfo descriptorPoolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };

    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.maxSets = 1;
    descriptorPoolInfo.pPoolSizes = &imageAttachmentPoolSize;

    VkDescriptorSetLayoutBinding bindings{};
    bindings.binding = 0;
    bindings.descriptorCount = attachmentSize;
    bindings.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

    descriptorLayoutInfo.bindingCount = 1;
    descriptorLayoutInfo.pBindings = &bindings;

    VkDescriptorPool pool;
    VkDescriptorSetLayout layout;
    vkCreateDescriptorPool(mActiveDevice, &descriptorPoolInfo, nullptr, &pool);
    vkCreateDescriptorSetLayout(mActiveDevice, &descriptorLayoutInfo, nullptr, &layout);

    VkDescriptorSetAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };

    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    vkAllocateDescriptorSets(mActiveDevice, &allocInfo, &mAttachmentDescriptorSet);

    return layout;
}

void MiniEngine::Backend::VulkanPipelineBuilder::updateSceneDescriptorSetData()
{
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = mSceneBlockBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(SceneBlock);

    VkWriteDescriptorSet writeSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    writeSet.dstSet = mSceneDescriptorSet;
    writeSet.dstBinding = 0;
    writeSet.dstArrayElement = 0;
    writeSet.descriptorCount = 1;
    writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeSet.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(mActiveDevice, 1, &writeSet, 0, nullptr);
}

void MiniEngine::Backend::VulkanPipelineBuilder::updateAttachmentDescriptorSetData(
    std::array<VulkanDriver::ImageAttachmentData, 5> &attachments)
{
    VkDescriptorImageInfo colorInfo{};
    colorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    colorInfo.imageView = attachments[0].imageView;

    VkDescriptorImageInfo positionInfo{};
    positionInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    positionInfo.imageView = attachments[1].imageView;

    VkDescriptorImageInfo normalInfo{};
    normalInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    normalInfo.imageView = attachments[2].imageView;

    VkDescriptorImageInfo roughnessInfo{};
    roughnessInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    roughnessInfo.imageView = attachments[3].imageView;

    auto imageInfos = {colorInfo, positionInfo, normalInfo, roughnessInfo};

    VkWriteDescriptorSet writeSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    writeSet.dstSet = mAttachmentDescriptorSet;
    writeSet.dstBinding = 0;
    writeSet.dstArrayElement = 0;
    writeSet.descriptorCount = 4;
    writeSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeSet.pImageInfo = imageInfos.begin();

    vkUpdateDescriptorSets(mActiveDevice, 1, &writeSet, 0, nullptr);
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

    vkCreateDescriptorSetLayout(mActiveDevice, &descriptorSetInfo, nullptr, &defaultLayout);

    return defaultLayout;
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

    vkCreateBuffer(mActiveDevice, &bufferCreateInfo, nullptr, &vertexBuffer);

    return vertexBuffer;
}

void MiniEngine::Backend::VulkanPipelineBuilder::instantiateTriangleBuffer()
{
	mDefaultTriangleBuffer = createDefaultTriangleBuffer();
    auto bufferMemory = mDriver->allocateBuffer(mDefaultTriangleBuffer);

    mDriver->pushBufferMemory(mDefaultTriangleBuffer,
                              bufferMemory,
                              mDefaultTriangleRenderableComponent.buffer.data(),
                              mDefaultTriangleRenderableComponent.buffer.size() * sizeof(float));
}
