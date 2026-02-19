#ifndef MINIENGINE_FRAMEGRAPH
#define MINIENGINE_FRAMEGRAPH

#include "pch.hpp"
#include <cstdint>
#include <functional>
#include <memory>
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

private:
    RenderGraph *graph;
    uint32_t index;
    std::function<void(VkCommandBuffer &)> build_render_pass_fn;
    std::function<void(VkClearDepthStencilValue *)> clear_depth_stencil_fn;
    std::function<void(VkClearColorValue *)> clear_value_fn;
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

    void setBackBufferSource(std::string name);

    // Validate
    // Traverse
    void bake() {}

private:
    std::vector<std::unique_ptr<RenderPass>> passes;
    std::vector<std::unique_ptr<RenderTextureResource>> textureResources;
};

RenderTextureResource &RenderPass::addColorOutput(std::string name, TextureDescription desc)
{
    auto res = graph->resolveTextureResource(name);
    res.writtenPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    return res;
}

RenderTextureResource &RenderPass::addDepthStencilOutput(std::string name, TextureDescription desc)
{
    auto res = graph->resolveTextureResource(name);
    res.writtenPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    return res;
}

RenderTextureResource &RenderPass::addAttachmentInput(std::string name)
{
    auto res = graph->resolveTextureResource(name);
    res.readInPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    return res;
}

} // namespace MiniEngine::Backend

#endif
