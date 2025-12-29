#ifndef MINIENGINE_FRAMEGRAPH
#define MINIENGINE_FRAMEGRAPH

#include "Logger.h"
#include "Scene.h"
#include "VulkanDriver.h"
#include "pch.hpp"
#include <cstdint>
#include <iterator>
#include <map>
#include <vulkan/vulkan_core.h>
#include <queue>

namespace MiniEngine::Backend {

struct Params {
    std::vector<ResourceRef> ShaderResources;
    std::vector<ResourceRef> RenderTargets;
};

class FrameGraphBuilder {

    // Can be further organized into render features (shadow passs/deferred...)
    void addPass(std::string name, Flags f, Params p, std::function<void()> execute){
    }

    // Setup phase
    // Go through all defined passes and check resources.
    // Communicate allocation of resources to graph
    TextureRef createTexture(TextureDesc& desc, std::string name, TextureFlags f){}

    BufferRef registerExternalBuffer(Buffer pooledBuffer, BufferFlags f){}

    // Exclude unreferenced ressources/passes
    // Compute/handle resource lifetimes
    // Fencing
    // Resource Allocation
    // Build graph
    void compile() {
    }

    // Execute the surviving passes-draw and dispatch
    // Access real API resources and set them in pipeline
    void execute(){}

    //submit multiple commands in vkqueuesubmit for each "dependency layer"
    // vkbingimagememory or whatever to place resources
    // alias memory using barriers and whatnot: https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/resource_aliasing.html
};

class FrameGraph {
};

} // namespace MiniEngine::Backend

#endif
