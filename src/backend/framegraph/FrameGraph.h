#ifndef MINIENGINE_FRAMEGRAPH
#define MINIENGINE_FRAMEGRAPH

#include "Logger.h"
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

    VkFormat format = VK_FORMAT_UNDEFINED;
    unsigned width = 0;
    unsigned height = 0;
    unsigned depth = 1;
    unsigned layers = 1;
    unsigned levels = 1;
    unsigned samples = 1;
    VkImageUsageFlags image_usage = 0;
    uint32_t physicalIndex = std::numeric_limits<uint32_t>::max();
    VkImageUsageFlags flags = 0;
};

// Vulkan Specific ===
struct RenderPassInfo
{
    const VkImageView *colorAttachments[4]; //change this
    const VkImageView *depthStencil = nullptr;
    uint32_t clearAttachments = 0;
    uint32_t loadAttachments = 0;
    uint32_t storeAttachments = 0;
};
//====================

class RenderPass;
struct PhysicalPass
{
    // std::vector<unsigned> passes;
    RenderPassInfo renderPassInfo;
    std::vector<std::tuple<RenderPass *, VkClearColorValue, uint32_t>> colorClearRequests;
    std::tuple<RenderPass *, VkClearDepthStencilValue> depthClearRequest;
    std::vector<uint32_t> physicalColorAttachments;
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
    RenderTextureResource &addColorOutput(
        std::string name, TextureDescription desc, const std::string &input = "");
    RenderTextureResource &addAttachmentInput(std::string name);
    RenderTextureResource &addStorageInput(std::string name, VkPipelineStageFlags2 stages);
    RenderTextureResource &addTextureInput(
        std::string name,
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
    std::vector<RenderTextureResource *> storageInputs;
    std::vector<RenderTextureResource *> storageOutputs;
    std::vector<RenderTextureResource *> colorInputs;
    std::vector<AccessedTextureResource> externalTextures;
    std::vector<AccessedTextureResource> externalStorageImages;
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
        auto it = std::find_if(
            textureResources.begin(),
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
        // for (auto &pass : passes) {
        //     if (pass->colorInputs.size() != pass->colorOutputs.size())
        //         ELOG("Size of inputs must match size of outputs"); // Why?

        //     // for (auto i = 0; i < pass->colorInputs.size(); ++i) {

        //     // }
        // }
    }

    void executeDependencyPass(
        const RenderPass &self,
        const std::unordered_set<uint32_t> &writtenPasses,
        uint32_t stackCount)
    {
        if (writtenPasses.empty())
            ELOG("No pass exists which writes to resource");

        if (stackCount > passes.size())
            ELOG("Cycle detected!");

        for (auto &pass : writtenPasses) {
            if (pass != self.index)
                passDependencies[self.index].insert(pass);
        }

        stackCount++;

        for (auto &pushedPass : writtenPasses) {
            if (pushedPass == self.index)
                ELOG("Pass depends on itself");

            passStack.push_back(pushedPass);
            auto &pass = *passes[pushedPass];
            traverseDependencies(pass, stackCount);
        }
    }

    void traverseDependencies(const RenderPass &pass, uint32_t stackSize)
    {
        if (pass.depthStencilInput != nullptr) {
            executeDependencyPass(pass, pass.depthStencilInput->writtenPasses, stackSize);
        }

        for (auto *input : pass.attachmentInputs) {
            executeDependencyPass(pass, input->writtenPasses, stackSize);
        }

        for (auto *input : pass.colorInputs) {
            executeDependencyPass(pass, input->writtenPasses, stackSize);
        }

        for (auto *input : pass.storageInputs) {
            executeDependencyPass(pass, input->writtenPasses, stackSize);
            executeDependencyPass(pass, input->readInPasses, stackSize);
        }
    }

    void filterPasses(std::vector<uint32_t> &list)
    {
        std::unordered_set<uint32_t> seen;

        auto outputItr = std::begin(list);
        for (auto itr = std::begin(list); itr != std::end(list); ++itr) {
            if (!seen.count(*itr)) {
                *outputItr = *itr;
                seen.insert(*itr);
                ++outputItr;
            }
        }

        list.erase(outputItr, std::end(list));
    }

    void reorderPasses(std::vector<uint32_t> flatPasses)
    {
        //for(auto &mergeDeps : passMergeDependencies)
    }

    void buildPhysicalResources()
    {
        uint32_t phyIndex = 0;

        for (auto &passIndex : passStack) {
            auto &pass = *passes[passIndex];

            for (auto &input : pass.externalTextures) {
                if (input.texture->physicalIndex == std::numeric_limits<uint32_t>::max()) {
                    physicalPassResources.push_back(input.texture);
                    input.texture->physicalIndex = phyIndex++;
                } else {
                    // Get from physical passes and add image/queue usage flags
                }
            }

            if (!pass.colorInputs.empty()) {
                for (auto i = 0; i < pass.colorInputs.size(); i++) {
                    auto *input = pass.colorInputs[i];
                    if (input) {
                        if (input->physicalIndex == std::numeric_limits<uint32_t>::max()) {
                            physicalPassResources.push_back(input);
                            input->physicalIndex = phyIndex++;
                        } else {
                            // same as above
                        }

                        if (pass.colorOutputs[i]->physicalIndex
                            == std::numeric_limits<uint32_t>::max())
                            pass.colorOutputs[i]->physicalIndex = input->physicalIndex;
                        else if (pass.colorOutputs[i]->physicalIndex != input->physicalIndex)
                            throw;
                    }
                }
            }

            if (!pass.storageInputs.empty()) {
                for (auto i = 0; i < pass.storageInputs.size(); i++) {
                    auto *input = pass.storageInputs[i];
                    if (input) {
                        if (input->physicalIndex == std::numeric_limits<uint32_t>::max()) {
                            physicalPassResources.push_back(input);
                            input->physicalIndex = phyIndex++;
                        } else {
                            // same as above
                        }

                        if (pass.storageOutputs[i]->physicalIndex
                            == std::numeric_limits<uint32_t>::max())
                            pass.storageOutputs[i]->physicalIndex = input->physicalIndex;
                        else if (pass.storageOutputs[i]->physicalIndex != input->physicalIndex)
                            throw;
                    }
                }
            }

            for (auto *output : pass.colorOutputs) {
                if (output->physicalIndex == std::numeric_limits<uint32_t>::max()) {
                    physicalPassResources.push_back(output);
                    output->physicalIndex = phyIndex++;
                } else {
                    // same as above
                }
            }

            for (auto *output : pass.storageOutputs) {
                if (output->physicalIndex == std::numeric_limits<uint32_t>::max()) {
                    physicalPassResources.push_back(output);
                    output->physicalIndex = phyIndex++;
                } else {
                    // same as above
                }
            }

            auto depthStencilInput = pass.depthStencilInput;
            auto depthStencilOutput = pass.depthStencilOutput;

            if (depthStencilInput) {
                if (depthStencilInput->physicalIndex == std::numeric_limits<uint32_t>::max()) {
                    physicalPassResources.push_back(depthStencilInput);
                    depthStencilInput->physicalIndex = phyIndex++;
                } else {
                    // once again as above
                }

                if (depthStencilOutput) {
                    if (depthStencilOutput->physicalIndex == std::numeric_limits<uint32_t>::max())
                        depthStencilOutput->physicalIndex = depthStencilInput->physicalIndex;
                    else
                        throw; //cannot alias, index already claimed

                    // once again as above
                }
            } else if (depthStencilOutput) {
                if (depthStencilOutput->physicalIndex == std::numeric_limits<uint32_t>::max()) {
                    physicalPassResources.push_back(depthStencilOutput);
                    depthStencilOutput->physicalIndex = phyIndex++;
                } else {
                    // again as the above
                }
            }

            //Attachment inputs alst so they can alias with color/depth attachments
            for (auto *input : pass.attachmentInputs) {
                if (input->physicalIndex == std::numeric_limits<uint32_t>::max()) {
                    physicalPassResources.push_back(input);
                    input->physicalIndex = phyIndex++;
                } else {
                    // Set queue and usae like before
                }
            }

            // Add "history" inputs here, ones that get saved from previous frame
        }
    }

    // void buildPhysicalPasses()
    // {
    //     physicalPasses.clear();
    //     PhysicalPass pPass;
    //
    //     // Lambdas will go in here to see if passes merge
    //
    //     for (auto index = 0; index < passStack.size();) {
    //         pPass.passes.push_back(passStack[index]);
    //         physicalPasses.push_back(pPass);
    //     }
    //
    //     for (auto &phyPass : physicalPasses) {
    //         auto index = static_cast<uint32_t>((&phyPass - physicalPasses.data()));
    //         for (auto &pass : phyPass.passes)
    //             passes[pass]->index = index;
    //     }
    // }

    void buildRenderPassInfo()
    {
        for (auto &pPass : physicalPasses) {
            auto &rp = pPass.renderPassInfo;
            rp.clearAttachments = 0;
            rp.loadAttachments = 0;
            rp.storeAttachments = ~0u; //?
            pPass.colorClearRequests.clear();
            pPass.depthClearRequest = {};

            auto &colors = pPass.physicalColorAttachments;
            colors.clear();

            for (auto passIndex : passStack) {
                auto &pass = passes[passIndex];

                for (auto i = 0; i < pass->colorOutputs.size(); ++i) {
                }
            }
        }
    }

    void bake()
    {
        validatePasses();

        auto framebuffer_itr = std::find_if(
            textureResources.begin(), textureResources.end(), [this](RenderTextureResource *res) {
                if (res->name == framebufferName)
                    return true;
                else
                    return false;
            });

        if (framebuffer_itr == textureResources.end())
            ELOG("Framebuffer not found during render graph baking.");

        passStack.clear();

        const auto *framebuffer = framebuffer_itr->get();

        if (framebuffer->writtenPasses.empty())
            ELOG("No pass exists which write to resource");

        for (auto &pass : framebuffer->writtenPasses)
            passStack.push_back(pass);

        auto tempStack = passStack;

        for (auto &pushedPass : tempStack) {
            auto &pass = *passes[pushedPass];
            traverseDependencies(pass, 0);
        }

        std::reverse(std::begin(passStack), std::end(passStack));
        filterPasses(passStack);

        //reorder_passes
        buildPhysicalResources();
        // buildPhysicalPasses();
        buildRenderPassInfo();
    }

private:
    std::vector<std::unique_ptr<RenderPass>> passes;
    std::vector<std::unique_ptr<RenderTextureResource>> textureResources;
    std::vector<std::unordered_set<uint32_t>> passDependencies;
    std::vector<std::unordered_set<uint32_t>> passMergeDependencies;
    std::vector<RenderTextureResource *> physicalPassResources;
    std::string framebufferName;
    std::vector<uint32_t> passStack;
    std::vector<PhysicalPass> physicalPasses;
};

RenderTextureResource &RenderPass::addColorOutput(
    std::string name, TextureDescription desc, const std::string &input)
{
    auto &res = graph->resolveTextureResource(name);
    res.writtenPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    res.width = desc.x;
    res.height = desc.y;
    res.format = desc.format;
    res.samples = desc.samples;
    colorOutputs.push_back(&res);

    if (!input.empty()) {
        auto &inputRes = graph->resolveTextureResource(input);
        inputRes.readInPasses.insert(index);
        inputRes.image_usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        colorInputs.push_back(&inputRes);
    } else
        colorInputs.push_back(nullptr);

    return res;
}

RenderTextureResource &RenderPass::addDepthStencilOutput(std::string name, TextureDescription desc)
{
    auto &res = graph->resolveTextureResource(name);
    res.writtenPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    res.width = desc.x;
    res.height = desc.y;
    res.format = desc.format;
    res.samples = desc.samples;
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

RenderTextureResource &RenderPass::addStorageInput(std::string name, VkPipelineStageFlags2 stages)
{
    auto &res = graph->resolveTextureResource(name);
    //TODO Add output too
    res.readInPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_STORAGE_BIT);

    AccessedTextureResource acc;
    acc.texture = &res;
    acc.layout = VK_IMAGE_LAYOUT_GENERAL;
    acc.access = VK_ACCESS_2_SHADER_STORAGE_READ_BIT; // and write
    acc.stages = stages;
    externalStorageImages.push_back(acc);
    return res;
}

RenderTextureResource &RenderPass::addTextureInput(std::string name, VkPipelineStageFlags2 stages)
{
    auto &res = graph->resolveTextureResource(name);
    res.readInPasses.insert(index);
    res.addImageUsage(VK_IMAGE_USAGE_SAMPLED_BIT);

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
