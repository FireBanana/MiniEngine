#ifndef VULKANRENDERPASS_H
#define VULKANRENDERPASS_H

#include <vector>

namespace MiniEngine::Backend {

class ImageAttachmentData;

class VulkanRenderPass
{
public:
    class Builder
    {
        Builder();
        void setImageAttachments(std::vector<ImageAttachmentData *>);
    };

private:
    VulkanRenderPass() = delete;
};
} // namespace MiniEngine::Backend
#endif
