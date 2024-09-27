#ifndef VULKANBARRIER_H
#define VULKANBARRIER_H

#include "VulkanHelper.h"

namespace MiniEngine::Backend {
class VulkanBarrier
{
public:
    class Builder
    {
    public:
        Builder() {}
        Builder(Builder &) = delete;
        Builder operator=(Builder &) = delete;
        Builder(Builder &&) = delete;
        Builder operator=(Builder &&) = delete;

        Builder &setImage(VkImage image);
        Builder &setBuffer(VkBuffer buffer);
        Builder &setCmdBuffer(VkCommandBuffer *cmd);
        Builder &setSrcAccessMask(VkAccessFlags flags);
        Builder &setDstAccessMask(VkAccessFlags flags);
        Builder &setAspectMask(VkImageAspectFlags flags);
        Builder &setOldLayout(VkImageLayout oldLayout);
        Builder &setNewLayout(VkImageLayout newLayout);
        Builder &setSrcStageMask(VkPipelineStageFlags srcStageMask);
        Builder &setDstStageMask(VkPipelineStageFlags dstStageMask);
        VulkanBarrier build();

        bool mHasImage = false;
        bool mHasBuffer = false;

        VkImage mImage;
        VkBuffer mBuffer;
        VkCommandBuffer *mCmdBuffer;
        VkAccessFlags mSrcAccessMask;
        VkAccessFlags mDstAccessMask;
        VkImageAspectFlags mAspectMask;
        VkImageLayout mOldLayout;
        VkImageLayout mNewLayout;
        VkPipelineStageFlags mSrcStageMask;
        VkPipelineStageFlags mDstStageMask;

    private:
    };
};
} // namespace MiniEngine::Backend
#endif // VULKANBARRIER_H
