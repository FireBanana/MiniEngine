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

enum class TextureUsage {
    COLOR_ATTACHMENT,
    DEPTH_ATTACHMENT,
};

struct ResourceHandle
{
    uint32_t handle;
    bool isValid() const { return handle != 1; }
};

struct ResourceBuffer
{
    VulkanBuffer *buffer;
};

struct TextureResourceDesc
{
    enum class Type { RENDER_TARGET, EXTERNAL, TEXTURE };
    Type type;
    std::string name;
    uint16_t width, height;
    uint8_t channels;
    uint8_t nativeHandle;
    VkFormat format;
    VulkanImage image;

    bool operator==(const TextureResourceDesc &rhs) { return this->name == rhs.name; }
};

struct VirtualResource
{
    TextureResourceDesc desc;
    uint32_t id;
    int refCount = 0;
    int firstPassId = -1;
    int lastPassId = -1;
    VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};

struct BufferResourceDesc
{
    enum class Type { UNIFORM, TRANSFER };
    std::string name;
    size_t size;
};

// struct ResourceEntry
// {
//     enum class Type { TEXTURE, BUFFER };
//     std::string name;
//     int id;
// };

struct RenderPassResource
{
    std::string name;
    std::vector<TextureResourceDesc> textureReadResources;
    std::vector<TextureResourceDesc> textureWriteResources;
    std::vector<BufferResourceDesc> bufferReadResources;
    std::vector<BufferResourceDesc> bufferWriteResources;

    // bool operator==(const RenderPassResource &rhs) { return this->name == rhs.name; }
};

struct RenderPass
{
    std::string name;
    std::vector<TextureResourceDesc> textureReadResources;
    std::vector<TextureResourceDesc> textureWriteResources;
    std::vector<BufferResourceDesc> bufferReadResources;
    std::vector<BufferResourceDesc> bufferWriteResources;

    std::function<void()> execute;
};

struct RenderPassNode
{
    std::string name;
    std::vector<uint32_t> reads;
    std::vector<uint32_t> writes;

    std::function<void(void *, VulkanDriver *)> execute;

    void *passData;
    bool isCulled = false;
    bool forceRequire = false; //In cases like writing to buffer for read back
    int refCount = 0;
};

//  Passed to setup lambda
class FrameGraphBuilder
{
public:
    uint32_t read(uint32_t input)
    {
        //add to current pass read list
        // return for chaining
        return input;
    }

    uint32_t write(uint32_t input)
    {
        // Writing creates a new version of the resource, or mark modified
        return input;
    }
};

class FrameGraph
{
public:
    FrameGraph(VulkanDriver *d)
        : driver(d)
    {}

    // Execute contains vulkan code and drawing commands, called from execute()
    template<typename Data, typename Setup, typename Execute>
    void addPass(std::string name, Setup setup, Execute execute)
    {
        RenderPassNode node;
        node.name = name;
        auto *data = new Data{};
        node.passData = data;

        // Setup
        FrameGraphBuilder builder{};
        setup(builder, *data);

        // Store Execute
        node.execute = [=](void *ddata, VulkanDriver *driver) {
            execute(*static_cast<Data *>(ddata), driver);
        };

        passes.push_back(node);
    }

    void execute(VulkanDriver *driver)
    {
        for (auto &pass : passes) {
            if (pass.isCulled)
                continue;

            //Transition resources
            for (auto handle : pass.reads) {
                transitionResource(driver, handle, VK_IMAGE_LAYOUT_GENERAL);
            }
            for (auto handle : pass.writes) {
                transitionResource(
                    driver, handle, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL); //Handle depth
            }

            pass.execute(pass.passData, driver);
        }
    }

    void transitionResource(VulkanDriver *driver, uint32_t resourceHandle, VkImageLayout target)
    {
        VirtualResource &res = resources[resourceHandle];
        if (res.currentLayout != target) {
            //INSERT BARRIER
            res.currentLayout = target;
        }
    }

    void bake(MiniEngine::Scene *scene)
    {
        std::vector<std::vector<int>> edges{passes.size()};
        std::vector<int> passDegrees(passes.size(), 0);

        // ==== Build dependencies - map resourceId -> index of last pass that wrote to it
        std::vector<int> resourceProducers(resources.size(), -1);

        for (int i = 0; i < passes.size(); ++i) {
            auto &pass = passes[i];

            // Who produces input for this pass
            for(auto inputHandle: pass.reads) {
                auto producerId = resourceProducers[inputHandle];

                if(producerId != -1 && producerId != i) {
                    //Dependency found: producerId -> i
                    edges[producerId].push_back(i);
                    passDegrees[i]++;
                }
            }

            // Register this pass as producer for its outputs
            for(auto outputHandle: pass.writes) {
                resourceProducers[outputHandle] = i;
            }
        }

        // ==== Culling
        cull();

        // ==== Kahn Sort
        std::queue<int> sortQueue;
        std::vector<int> sortedPasses;

        // Find initial passes that are not culled
        for(int i = 0; i < passes.size(); ++i) {
            if(!passes[i].isCulled && passDegrees[i] == 0)
                sortQueue.push(i);
        }

        debugDrawGraph(order);
    }

    void cull()
    {
        // Reset ref counts
        for (auto &pass : passes)
            pass.refCount = pass.forceRequire ? 1 : 0;
        for (auto &res : resources)
            res.refCount = 0;

        resources[backBufferHandle].refCount = 1;

        // Iterate backwards
        for (int i = passes.size() - 1; i >= 0; --i) {
            auto &pass = passes[i];

            bool isNeeded = (pass.refCount > 0);
            for (auto outId : pass.writes) {
                if (resources[outId].refCount > 0)
                    isNeeded = true;
            }

            if (isNeeded) {
                pass.isCulled = false;

                for (auto inId : pass.reads) {
                    resources[inId].refCount++;
                }
            } else {
                pass.isCulled = true;
            }
        }
    }

    void calculateLifetimes(std::vector<int> sortedPasses)
    {
        for(auto i = 0; i < sortedPasses.size(); ++i) {
            auto &pass = passes[sortedPasses[i]];
            if(pass.isCulled) continue;

            auto touchResource = [&](uint32_t handle) {
                if(resources[handle].firstPassIndex == -1) resources[handle].firstPassIndex = i;
                resources[handle].lastPassIndex = i;
            }

            for(auto h: pass.reads) touchResource(h);
            for(auto h: pass.writes) touchResource(h);
        }
    }

    void resolveMemory() {
        std::vector<VulkanImage> freePool;

        for(int i = 0; i < sortedPasses.size(); ++i){
            // Free resources unused from previous step
            for(auto &res : resources) {
                if(res.lastPassIndex == i - 1) {
                    freePool.push_back(res.physicalImage);
                }
            }
        }

        // Allocate resources that satisfy criteria
    }

    // External?
    int addResource(RenderPass *pass, VulkanImage *image) { return 0; }
    int addResource(RenderPass *pass, VulkanBuffer *buffer) { return 0; }

    std::unique_ptr<VulkanDriver> driver;
    std::vector<VirtualResource> resources;
    std::vector<RenderPassNode> passes;

    void debugDrawGraph(std::vector<int> indegree)
    {
        if (indegree.empty()) {
            std::cout << "Empty graph\n";
            return;
        }

        std::cout << "\n=== Framegraph ===\n\n";

        // Print horizontal flow
        std::cout << "Flow: ";
        for (size_t i = 0; i < indegree.size(); ++i) {
            std::cout << "[" << passes[indegree[i]].name << "]";
            if (i < indegree.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << "\n\n";

        // Print vertical visualization with levels
        std::cout << "Levels:\n";
        for (size_t i = 0; i < indegree.size(); ++i) {
            std::cout << "Level " << i << ": ";

            // Indentation for visual effect
            for (size_t j = 0; j < i; ++j) {
                std::cout << "  ";
            }

            std::cout << "+---+\n";
            std::cout << "         ";
            for (size_t j = 0; j < i; ++j) {
                std::cout << "  ";
            }
            std::cout << "| " << passes[indegree[i]].name << " |\n";
            std::cout << "         ";
            for (size_t j = 0; j < i; ++j) {
                std::cout << "  ";
            }
            std::cout << "+---+\n";

            if (i < indegree.size() - 1) {
                std::cout << "           ";
                for (size_t j = 0; j < i; ++j) {
                    std::cout << "  ";
                }
                std::cout << "|\n           ";
                for (size_t j = 0; j < i; ++j) {
                    std::cout << "  ";
                }
                std::cout << "v\n";
            }
        }

        std::cout << "\n===========================================\n";
    }
};

} // namespace MiniEngine::Backend

#endif
