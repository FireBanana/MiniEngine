#include "Logger.h"
#include "VulkanDriver.h"
#include "pch.hpp"
#include <winuser.h>
#include <queue>

namespace MiniEngine::Backend {

enum class TextureUsage {
    COLOR_ATTACHMENT,
    DEPTH_ATTACHMENT,
};

// Loaded through the API as a texture
struct ResourceImage
{
    VulkanImage *image;
};

struct ResourceBuffer
{
    VulkanBuffer *buffer;
};

struct TextureResourceDesc
{
    // enum class Type { RENDER_TARGET, INPUT_ATTACHMENT, OUTPUT_STORAGE, EXTERNAL, DEPTH };
    // Type type;
    std::string name;
    uint16_t width, height;
    uint8_t channels;
    uint8_t nativeHandle;
    VkFormat format;

    bool operator==(const TextureResourceDesc &rhs) { return this->name == rhs.name; }
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

    bool operator==(const RenderPassResource &rhs) { return this->name == rhs.name; }
};

struct RenderPass
{
    int id;
    std::vector<TextureResourceDesc> textureReadResources;
    std::vector<TextureResourceDesc> textureWriteResources;
    std::vector<BufferResourceDesc> bufferReadResources;
    std::vector<BufferResourceDesc> bufferWriteResources;

    std::function<void()> execute;
};

class FrameGraph
{
public:
    FrameGraph() {}

    // Execute contains vulkan code and drawing commands, called from execute()
    void addPass(std::string name, RenderPassResource passRes, std::function<void()> execute)
    {
        // Set Attachments
        // Set RenderTargets

        auto pass = RenderPass{};
        pass.textureReadResources = passRes.textureReadResources;
        pass.textureWriteResources = passRes.textureWriteResources;
        pass.bufferReadResources = passRes.bufferReadResources;
        pass.bufferWriteResources = passRes.bufferWriteResources;

        pass.execute = execute;
    }

    int createTexture(TextureResourceDesc desc)
    {
        auto texture = driver->createTexture(
            desc.width, desc.height, desc.channels, nullptr, VulkanDriver::TextureType::Default);

        // switch (desc.type) {
        // case TextureResourceDesc::Type::RENDER_TARGET:
        //     break;
        // case TextureResourceDesc::Type::INPUT_ATTACHMENT:
        //     break;
        // case TextureResourceDesc::Type::OUTPUT_STORAGE:
        //     break;
        // case TextureResourceDesc::Type::DEPTH:
        //     break;
        // }
        return 0;
    }

    void bake()
    {
        //Build edges
        std::vector<std::vector<int>> edges(passes.size());

        // Build dependencies
        for (auto i = 0; i < passes.size(); ++i) {
            for (auto j = 0; j < passes.size(); ++j) {
                if (i == j)
                    continue;

                for (auto write : passes[i].textureWriteResources) {
                    // Write -> Read
                    if (std::find(
                            passes[j].textureReadResources.begin(),
                            passes[j].textureReadResources.end(),
                            write)
                        != passes[j].textureWriteResources.end()) {
                        edges[i].push_back(j);
                    }

                    // Write -> Write
                    if (std::find(
                            passes[j].textureWriteResources.begin(),
                            passes[j].textureWriteResources.end(),
                            write)
                        != passes[j].textureWriteResources.end()) {
                        edges[i].push_back(j);
                    }
                }
            }
        }

        // Kahn Sort
        std::vector<int> indegree(edges.size(), 0);

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
        //Sorted

        for(auto i : order)
            MiniEngine::Logger::print("Pass: {}", i);
    }

    // External?
    int addResource(RenderPass *pass, VulkanImage *image) { return 0;}
    int addResource(RenderPass *pass, VulkanBuffer *buffer) { return 0;}

    std::unique_ptr<VulkanDriver> driver;
    std::vector<RenderPass> passes;

    void debugDrawGraph() {}
};

} // namespace MiniEngine::Backend
