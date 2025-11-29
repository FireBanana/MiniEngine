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
                transitionResource(driver, handle, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL); //Handle depth
            }

            pass.execute(pass.passData, driver);
        }
    }

    void transitionResource(VulkanDriver *driver, uint32_t resourceHandle, VkImageLayout target) 
    {
        VirtualResource &res = resources[resourceHandle];
        if(res.currentLayout != target) {
            //INSERT BARRIER
            res.currentLayout = target;
        }
    }

    int createTexture(TextureResourceDesc desc)
    {
        auto texture = driver->createTexture(
            desc.width, desc.height, desc.channels, nullptr, VulkanDriver::TextureType::Default);

        switch (desc.type) {
        case TextureResourceDesc::Type::RENDER_TARGET:
            // driver->getCurrentRenderTarget();
            break;
        case TextureResourceDesc::Type::TEXTURE:
            // driver->createTexture();
            break;
        case TextureResourceDesc::Type::EXTERNAL:
            // driver->getTextureId();
            break;
        }
        return 0;
    }

    void bake(MiniEngine::Scene *scene)
    {
        //Build edges
        std::vector<std::vector<int>> edges(passes.size());

        // Build dependencies
        for (auto i = 0; i < passes.size(); ++i) {
            for (auto j = i; j < passes.size(); ++j) {
                if (i == j) //Check self dependency issues
                    continue;

                // Write checks
                for (auto write : passes[i].textureWriteResources) {
                    // Write -> Write
                    if (std::find(
                            passes[j].textureWriteResources.begin(),
                            passes[j].textureWriteResources.end(),
                            write)
                        != passes[j].textureWriteResources.end()) {
                        edges[i].push_back(j);
                    }
                    // Write -> Read
                    if (std::find(
                            passes[j].textureReadResources.begin(),
                            passes[j].textureReadResources.end(),
                            write)
                        != passes[j].textureReadResources.end()) {
                        edges[i].push_back(j);
                    }
                }

                // Read checks
                for (auto read : passes[i].textureReadResources) {
                    // Write -> Read
                    if (std::find(
                            passes[j].textureWriteResources.begin(),
                            passes[j].textureWriteResources.end(),
                            read)
                        != passes[j].textureWriteResources.end()) {
                        edges[j].push_back(i);
                    }
                }
            }
        }

        // Kahn Sort
        std::vector<int> indegree(passes.size(), 0);

        for (int i = 0; i < edges.size(); ++i) {
            for (int j : edges[i])
                indegree[j]++;
        }

        std::queue<int> q;
        for (int i = 0; i < edges.size(); ++i)
            if (indegree[i] == 0)
                q.push(i);

        std::vector<int> order;
        while (!q.empty()) {
            auto u = q.front();
            q.pop();
            order.push_back(u);

            for (auto e : edges[u]) {
                if (--indegree[e] == 0)
                    q.push(e);
            }
        }

        //Create resources

        //Execute setups
        for (auto o : order) {
            passes[o].execute();
        }

        debugDrawGraph(order);
    }

    void cull()
    {

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
