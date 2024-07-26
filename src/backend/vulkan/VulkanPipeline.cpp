#include "VulkanPipeline.h"
#include "EnumExtension.h"
#include "GlslCompiler.h"
#include "Logger.h"
#include "VulkanDriver.h"
#include <numeric>

MiniEngine::Backend::VulkanPipeline::Builder::Builder(VulkanDriver *driver)
    : mDriver{driver}
    , mVertexInputStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO}
{}

MiniEngine::Backend::VulkanPipeline::Builder &
MiniEngine::Backend::VulkanPipeline::Builder::addVertexAttributeState(int binding,
                                                                      std::vector<unsigned int> attr)
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

    std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions{1};
    vertexBindingDescriptions[0] = {};
    vertexBindingDescriptions[0].binding = binding;
    vertexBindingDescriptions[0].stride = std::accumulate(attr.begin(), attr.end(), 0)
                                          * sizeof(float);
    vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescription{};
    int offset = 0;

    for (auto i = 0; i < attr.size(); ++i) {
        VkVertexInputAttributeDescription value{};
        value.location = i;
        value.binding = vertexBindingDescriptions[0].binding;
        value.format = attr[i] == 3 ? VK_FORMAT_R32G32B32_SFLOAT
                                    : VK_FORMAT_R32G32_SFLOAT; // either 3 or 2
        value.offset = offset * sizeof(float);

        vertexAttributeDescription.push_back(value);

        offset += attr[i];
    }

    mVertexInputStateCreateInfo = vertexInputInfo;
    mVertexBindingDescriptions = vertexBindingDescriptions;
    mVertexAttributeDescription = vertexAttributeDescription;

    return *this;
}

MiniEngine::Backend::VulkanPipeline::Builder &
MiniEngine::Backend::VulkanPipeline::Builder::addDescriptorSet(VulkanDescriptorSet &&set)
{
    mDescriptors.push_back(set);
    return *this;
}

MiniEngine::Backend::VulkanPipeline::Builder &
MiniEngine::Backend::VulkanPipeline::Builder::addShaderState(const char *vert, const char *frag)
{
    mShaderStages[0] = VkPipelineShaderStageCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    mShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    mShaderStages[0].module = MiniTools::GlslCompiler::loadShader(vert,
                                                                  VK_SHADER_STAGE_VERTEX_BIT,
                                                                  mDriver->mActiveDevice);

    mShaderStages[0].pName = "main";

    mShaderStages[1] = VkPipelineShaderStageCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    mShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    mShaderStages[1].module = MiniTools::GlslCompiler::loadShader(frag,
                                                                  VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                  mDriver->mActiveDevice);

    mShaderStages[1].pName = "main";

    return *this;
}

MiniEngine::Backend::VulkanPipeline::Builder &
MiniEngine::Backend::VulkanPipeline::Builder::setAttachmentCount(int count)
{
    mAttachmentCount = count;
    return *this;
}

MiniEngine::Backend::VulkanPipeline::Builder &
MiniEngine::Backend::VulkanPipeline::Builder::setDepthState(bool depthTest, bool depthWrite)
{
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depthStencilInfo.depthTestEnable = depthTest;
    depthStencilInfo.depthWriteEnable = depthWrite;
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;

    mDepthStencilStateCreateInfo = depthStencilInfo;

    return *this;
}

MiniEngine::Backend::VulkanPipeline::Builder &
MiniEngine::Backend::VulkanPipeline::Builder::setRasterState(bool cullBack, bool clockWise)
{
    VkPipelineRasterizationStateCreateInfo rasterInfo{
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterInfo.cullMode = cullBack ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
    rasterInfo.frontFace = clockWise ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterInfo.lineWidth = 1.0f;

    mRasterStateCreateInfo = rasterInfo;

    return *this;
}

MiniEngine::Backend::VulkanPipeline::Builder &
MiniEngine::Backend::VulkanPipeline::Builder::setDynamicState(
    std::vector<VkDynamicState> &&dynamicState)
{
    VkPipelineDynamicStateCreateInfo dynamicInfo{
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamicInfo.pDynamicStates = dynamicState.data();
    dynamicInfo.dynamicStateCount = static_cast<uint32_t>(dynamicState.size());

    mDynamicStateCreateInfo = dynamicInfo;

    return *this;
}

MiniEngine::Backend::VulkanPipeline MiniEngine::Backend::VulkanPipeline::Builder::build()
{
    VulkanPipeline pipeline;

    // Make separate so not created again
    VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    layoutInfo.setLayoutCount = mDescriptors.size();

    std::vector<VkDescriptorSetLayout> setLayouts{};
    setLayouts.reserve(mDescriptors.size());

    for (auto &d : mDescriptors) {
        setLayouts.push_back(*d.getDescriptorSetLayout());
    }

    layoutInfo.pSetLayouts = setLayouts.data();
    vkCreatePipelineLayout(mDriver->mActiveDevice, &layoutInfo, nullptr, &mLayout);

    mVertexInputStateCreateInfo.vertexBindingDescriptionCount = mVertexBindingDescriptions.size();
    mVertexInputStateCreateInfo.pVertexBindingDescriptions = mVertexBindingDescriptions.data();
    mVertexInputStateCreateInfo.vertexAttributeDescriptionCount = mVertexAttributeDescription.size();
    mVertexInputStateCreateInfo.pVertexAttributeDescriptions = mVertexAttributeDescription.data();

    VkPipelineColorBlendAttachmentState colorBlendState{};
    colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                                     | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    std::vector<VkPipelineColorBlendAttachmentState> blendList;

    for (int i = 0; i < mAttachmentCount; ++i)
        blendList.push_back(colorBlendState);

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    colorBlendInfo.attachmentCount = blendList.size();
    colorBlendInfo.pAttachments = blendList.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineViewportStateCreateInfo viewportInfo{
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportInfo.viewportCount = 1;
    viewportInfo.scissorCount = 1;

    VkPipelineMultisampleStateCreateInfo multiSampleInfo{
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multiSampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkFormat> colorFormatList;

    for (int i = 0; i < mAttachmentCount; ++i)
        colorFormatList.push_back(mDriver->mActiveSwapchain.getFormat());

    VkPipelineRenderingCreateInfo renderingInfo{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    renderingInfo.colorAttachmentCount = colorFormatList.size();
    renderingInfo.pColorAttachmentFormats = colorFormatList.data();
    renderingInfo.depthAttachmentFormat = mDriver->mActiveSwapchain.getDepthFormat();

    VkGraphicsPipelineCreateInfo pipelineInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipelineInfo.stageCount = static_cast<uint32_t>(mShaderStages.size());
    pipelineInfo.pVertexInputState = &mVertexInputStateCreateInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineInfo.pRasterizationState = &mRasterStateCreateInfo;
    pipelineInfo.pColorBlendState = &colorBlendInfo;
    pipelineInfo.pMultisampleState = &multiSampleInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pDepthStencilState = &mDepthStencilStateCreateInfo;
    pipelineInfo.pDynamicState = &mDynamicStateCreateInfo;
    pipelineInfo.pStages = mShaderStages.data();
    pipelineInfo.layout = mLayout;
    pipelineInfo.pNext = &renderingInfo;

    vkCreateGraphicsPipelines(mDriver->mActiveDevice,
                              VK_NULL_HANDLE,
                              1,
                              &pipelineInfo,
                              nullptr,
                              &pipeline.mPipeline);

    pipeline.mDescriptors = std::move(mDescriptors);
    pipeline.mPipelineLayout = mLayout;
    pipeline.mDriver = mDriver;

    return pipeline;
}

void MiniEngine::Backend::VulkanPipeline::bind(VkCommandBuffer cmd)
{
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

    for (auto &d : mDescriptors)
        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                mPipelineLayout,
                                0,
                                1,
                                d.getDescriptorSet(),
                                0,
                                nullptr);
}
