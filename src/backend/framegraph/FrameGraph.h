#ifndef MINIENGINE_FRAMEGRAPH
#define MINIENGINE_FRAMEGRAPH

#include "Logger.h"
#include "pch.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <stack>
#include <unordered_set>
#include <vulkan/vulkan_core.h>

namespace MiniEngine::Backend {

struct TextureDescription
{
    float x;
    float y;
    VkFormat format = VK_FORMAT_UNDEFINED;
    uint8_t samples = 1;
    bool persistent = true;
};

struct BufferDescription
{
    VkDeviceSize size = 0;
    VkBufferUsageFlags useFlags = 0;
    bool persistent = true;
};

struct RenderResource
{
    std::string name;
    std::unordered_set<uint32_t> writtenPasses;
    std::unordered_set<uint32_t> readInPasses;
};

struct RenderTextureResource : RenderResource
{
    void addImageUsage(VkImageUsageFlags f) { flags |= f; }

    VkImageUsageFlags flags = 0;
};

struct AccessedTextureResource
{
    RenderTextureResource *texture = nullptr;
    VkPipelineStageFlags2 stages = 0;
    VkAccessFlags2 access = 0;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

class RenderGraph;
class RenderPass
{
public:
    RenderTextureResource &addColorOutput(std::string name, TextureDescription desc);
    RenderTextureResource &addAttachmentInput(std::string name);
    RenderTextureResource &addTextureInput(std::string name,
                                           VkPipelineStageFlags2 stages); //External texture
    RenderTextureResource &addDepthStencilInput(std::string name);
    RenderTextureResource &addDepthStencilOutput(std::string name, TextureDescription desc);

    // Callbacks called every frame to do work
    void setBuildRenderPass(std::function<void(VkCommandBuffer &)> fn)
    {
        build_render_pass_fn = std::move(fn);
    }

    void setGetClearDepthStencil(std::function<void(VkClearDepthStencilValue *)> fn)
    {
        clear_depth_stencil_fn = std::move(fn);
    }

    void setGetClearColor(std::function<void(VkClearColorValue *)> fn)
    {
        clear_value_fn = std::move(fn);
    }

    RenderGraph *graph;
    uint32_t index;
    std::function<void(VkCommandBuffer &)> build_render_pass_fn;
    std::function<void(VkClearDepthStencilValue *)> clear_depth_stencil_fn;
    std::function<void(VkClearColorValue *)> clear_value_fn;

    std::vector<RenderTextureResource *> colorOutputs;
    std::vector<RenderTextureResource *> attachmentInputs;
    std::vector<RenderTextureResource *> colorInputs;
    std::vector<AccessedTextureResource> externalTextures;
    RenderTextureResource *depthStencilInput = nullptr;
    RenderTextureResource *depthStencilOutput = nullptr;
};

class RenderGraph
{
public:
    RenderPass *addPass(std::string name)
    {
        passes.emplace_back(new RenderPass{});
        return passes.back().get();
    }

    RenderTextureResource &resolveTextureResource(std::string name)
    {
        auto it = std::find_if(textureResources.begin(),
                               textureResources.end(),
                               [&name](std::unique_ptr<RenderTextureResource> pred) {
                                   if (pred->name == name)
                                       return true;
                                   else
                                       return false;
                               });

        if (it != textureResources.end()) { // Texture found
            return **(it);
        } else {
            textureResources.emplace_back(new RenderTextureResource);
            return *textureResources.back();
        }
    }

    void setBackBufferSource(std::string name)
    {
        // Check if it exists
        framebufferName = name;
    }

    void validatePasses()
    {
        for (auto &pass : passes) {
            if (pass->colorInputs.size() != pass->colorOutputs.size())
                ELOG("Size of inputs must match size of outputs"); // Why?

            // for (auto i = 0; i < pass->colorInputs.size(); ++i) {

            // }
        }
    }

    void traverseDependencies(const RenderPass &pass, uint32_t stackSize)
    {

    }

    void bake()
    {
        validatePasses();

        auto framebuffer_itr = std::find_if(textureResources.begin(),
                                            textureResources.end(),
                                            [this](RenderTextureResource *res) {
                                                if (res->name == framebufferName)
                                                    return true;
                                                else
                                                    return false;
                                            });

        if (framebuffer_itr == textureResources.end())
            ELOG("Framebuffer not found during render graph baking.");

        passStack.clear();

        const auto *framebuffer = framebuffer_itr->get();

        if(framebuffer->writtenPasses.empty())
            ELOG("No pass exists which write to resource");

        for(auto &pass : framebuffer->writtenPasses)
            passStack.push_back(pass);

        auto tempStack = passStack;

        for(auto &pushedPass : tempStack) {
            auto &pass = *passes[pushedPass];
            traverseDependencies(pass, 0);
        }
    }

private:
    std::vector<std::unique_ptr<RenderPass>> passes;
    std::vector<std::unique_ptr<RenderTextureResource>> textureResources;
    std::string framebufferName;
    std::vector<uint32_t> passStack;
};

RenderTextureResource &RenderPass::addColorOutput(std::string name, TextureDescription desc)
{
    auto &res = graph->resolveTextureResource(name);
    res.writtenPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    colorOutputs.push_back(&res);
    return res;
}

RenderTextureResource &RenderPass::addDepthStencilOutput(std::string name, TextureDescription desc)
{
    auto &res = graph->resolveTextureResource(name);
    res.writtenPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    depthStencilOutput = &res;
    return res;
}

RenderTextureResource &RenderPass::addAttachmentInput(std::string name)
{
    auto &res = graph->resolveTextureResource(name);
    res.readInPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    attachmentInputs.push_back(&res);
    return res;
}

RenderTextureResource &RenderPass::addDepthStencilInput(std::string name)
{
    auto &res = graph->resolveTextureResource(name);
    res.readInPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    depthStencilInput = &res;
    return res;
}

RenderTextureResource &RenderPass::addTextureInput(std::string name, VkPipelineStageFlags2 stages)
{
    auto &res = graph->resolveTextureResource(name);
    res.readInPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_STORAGE_BIT);

    AccessedTextureResource acc;
    acc.texture = &res;
    acc.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    acc.access = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;

    if (stages != 0)
        acc.stages = stages;
    else
        acc.stages = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    externalTextures.push_back(acc);
    return res;
}

} // namespace MiniEngine::Backend

#endif
