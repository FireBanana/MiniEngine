#pragma once

#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "VulkanHelper.h"

namespace MiniEngine::Backend {

class VulkanDriver;

class VulkanSwapchain
{
public:
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

private:
    VkSwapchainKHR mSwapchain;

    friend class VulkanDriver;
};
} // namespace MiniEngine::Backend

#endif // VULKANSWAPCHAIN_H
