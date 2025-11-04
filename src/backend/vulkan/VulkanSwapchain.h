#pragma once

#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "VulkanHelper.h"

namespace MiniEngine::Backend {

class VulkanDriver;

class VulkanSwapchain
{
public:
    struct PerFrameData // Swapchain frame
    {
        VkImage rawImage;
        VkImageView imageView;
    };

    struct PerFrameInFlightData
    {
        VkCommandPool imageCommandPool;
        VkCommandBuffer imageCommandBuffer;
    };

    class Builder
    {
    public:
        Builder(VulkanDriver *driver);
        Builder(Builder &) = delete;
        Builder operator=(Builder &) = delete;
        Builder(Builder &&) = delete;
        Builder operator=(Builder &&) = delete;

        Builder &setPreferredFormat(VkFormat format);
        Builder &setPreferredDepthFormat(VkFormat format);
        Builder &setPresentMode(VkPresentModeKHR presentMode);
        Builder &setSurface(VkSurfaceKHR surface);
        Builder &setWidth(float width);
        Builder &setHeight(float height);
        VulkanSwapchain build();

    private:
        VulkanDriver *mDriver;
        VkFormat mColorFormat;
        VkFormat mDepthFormat;
        VkPresentModeKHR mPresentMode;
        VkSurfaceKHR mSurface;
        float mWidth;
        float mHeight;
    };

    VkSwapchainKHR getSwapchain() const { return mSwapchain; }
    VkFormat getFormat() const { return mColorFormat; }
    VkFormat getDepthFormat() const { return mDepthFormat; }
    uint32_t getSwapchainSize() const { return mSwapchainCount; }
    std::vector<PerFrameData> getPerFrameData() const { return mSwapchainPerFrameData; }
    std::vector<PerFrameInFlightData> getPerFrameInFlightData() const { return mFrameInFlightData; }

private:
    VulkanDriver *mDriver;
    VkSwapchainKHR mSwapchain;
    VkFormat mColorFormat;
    VkFormat mDepthFormat;
    uint32_t mSwapchainCount;
    std::vector<PerFrameData> mSwapchainPerFrameData;
    std::vector<PerFrameInFlightData> mFrameInFlightData;

    void createPerFrameData();

    friend class VulkanDriver;
};
} // namespace MiniEngine::Backend

#endif // VULKANSWAPCHAIN_H
