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
    enum class Type { RENDER_TARGET, INPUT_ATTACHMENT, OUTPUT_STORAGE, DEPTH };
    Type type;
    std::string name;
    uint16_t width, height;
    uint8_t channels;
    VkFormat format;
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
    std::vector<TextureResourceDesc> attachments;
    std::vector<BufferResourceDesc> buffers;
};

struct GraphNode
{
    int id;
    std::vector<TextureResourceDesc *> textureReadResources;
    std::vector<TextureResourceDesc *> textureWriteResources;
    std::vector<BufferResourceDesc *> bufferReadResources;
    std::vector<BufferResourceDesc *> bufferWriteResources;
};

class FrameGraph
{
public:
    FrameGraph() {}

    // Execute contains vulkan code
    void addPass(std::string name, RenderPassResource pass, std::function<void()> execute)
    {
        // Set Attachments
        // Set RenderTargets

        auto node = GraphNode{};

        for (auto &attachment : pass.attachments) {
        }
    }

    int createTexture(TextureResourceDesc desc)
    {
        auto texture = driver->createTexture(
            desc.width, desc.height, desc.channels, nullptr, VulkanDriver::TextureType::Default);

        switch (desc.type) {
        case TextureResourceDesc::Type::RENDER_TARGET:
            break;
        case TextureResourceDesc::Type::INPUT_ATTACHMENT:
            break;
        case TextureResourceDesc::Type::OUTPUT_STORAGE:
            break;
        case TextureResourceDesc::Type::DEPTH:
            break;
        }
    }

    // External?
    int addResource(VulkanImage *image) {}
    int addResource(VulkanBuffer *buffer) {}

    std::unique_ptr<VulkanDriver> driver;

    std::vector<GraphNode> passes;
};

} // namespace MiniEngine::Backend
