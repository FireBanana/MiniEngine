#include "VulkanDriver.h"
#include "pch.hpp"
#include <winuser.h>

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
    enum class Type { RENDER_TARGET, INPUT_ATTACHMENT, DEPTH };
    Type type;
    std::string name;
    uint32_t width, height;
    VkFormat format;
};

struct BufferResourceDesc
{
    enum class Type { UNIFORM, TRANSFER };
    std::string name;
    size_t size;
};

struct ResourceEntry
{
    enum class Type {TEXTURE, BUFFER};
    std::string name;
    int id;

};

struct RenderPass
{
    std::string name;
    std::vector<TextureResourceDesc> attachments;
    std::vector<BufferResourceDesc> buffers;
};

struct GraphNode
{
    int id;
    std::vector<ResourceEntry *> readResources;
    std::vector<ResourceEntry *> writeResources;
};

class FrameGraph
{
public:
    FrameGraph() {}

    void addPass(std::string name, RenderPass pass)
    {
        // Set Attachments
        // Set RenderTargets

        for (auto &attachment : pass.attachments) {
            switch (attachment.type) {
                case TextureResourceDesc::Type::RENDER_TARGET: break;
                case TextureResourceDesc::Type::INPUT_ATTACHMENT: break;
                case TextureResourceDesc::Type::DEPTH: break;
            }
        }
    }

    void addResource(VulkanImage *image) {}
    void addResource(VulkanBuffer *buffer) {}

    std::unique_ptr<VulkanDriver *> driver;

    std::vector<ResourceEntry> resources;
    std::vector<GraphNode> passes;
};

} // namespace MiniEngine::Backend
