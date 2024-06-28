#include "VulkanSwapchain.h"
#include "VulkanDriver.h"

MiniEngine::Backend::VulkanSwapchain::Builder::Builder(VulkanDriver *driver)
    : mDriver(driver)
{}

MiniEngine::Backend::VulkanSwapchain::Builder &
MiniEngine::Backend::VulkanSwapchain::Builder::setPreferredFormat(VkFormat format)
{
    mColorFormat = format;
    return *this;
}

MiniEngine::Backend::VulkanSwapchain::Builder &
MiniEngine::Backend::VulkanSwapchain::Builder::setPreferredDepthFormat(VkFormat format)
{
    mDepthFormat = format;
    return *this;
}

MiniEngine::Backend::VulkanSwapchain::Builder &
MiniEngine::Backend::VulkanSwapchain::Builder::setPresentMode(VkPresentModeKHR presentMode)
{
    mPresentMode = presentMode;
    return *this;
}

MiniEngine::Backend::VulkanSwapchain::Builder &
MiniEngine::Backend::VulkanSwapchain::Builder::setSurface(VkSurfaceKHR surface)
{
    mSurface = surface;
    return *this;
}

MiniEngine::Backend::VulkanSwapchain::Builder &
MiniEngine::Backend::VulkanSwapchain::Builder::setWidth(float width)
{
    mWidth = width;
    return *this;
}

MiniEngine::Backend::VulkanSwapchain::Builder &
MiniEngine::Backend::VulkanSwapchain::Builder::setHeight(float height)
{
    mHeight = height;
    return *this;
}

MiniEngine::Backend::VulkanSwapchain MiniEngine::Backend::VulkanSwapchain::Builder::build()
{
    VulkanSwapchain vSwapchain{};

    VkSurfaceCapabilitiesKHR surfaceProperties;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDriver->mActiveGpu, mSurface, &surfaceProperties);

    // Preferred format VK_FORMAT_R16G16B16A16_SFLOAT
    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(mDriver->mActiveGpu,
                                         mSurface,
                                         &surfaceFormatCount,
                                         nullptr);
    std::vector<VkSurfaceFormatKHR> supportedFormatList(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(mDriver->mActiveGpu,
                                         mSurface,
                                         &surfaceFormatCount,
                                         supportedFormatList.data());

    auto iter = std::find_if(supportedFormatList.begin(),
                             supportedFormatList.end(),
                             [](VkSurfaceFormatKHR currFormat) {
                                 return currFormat.format == mColorFormat;
                             });

    if (iter == supportedFormatList.end()) {
        iter = supportedFormatList.begin();
        MiniEngine::Logger::wprint("{} not found as a supported format. Defaulting to {}",
                                   mColorFormat,
                                   (*iter).format);
    }

    auto format = *iter;

    mColorFormat = format.format;

    // Choose desired depth as well
    constexpr VkFormat depthList[] = {VK_FORMAT_D32_SFLOAT,
                                      VK_FORMAT_D32_SFLOAT_S8_UINT,
                                      VK_FORMAT_D24_UNORM_S8_UINT};

    mDepthFormat = depthList[0];

    for (auto dFormat : depthList) {
        VkFormatProperties dProps;
        vkGetPhysicalDeviceFormatProperties(mDriver->mActiveGpu, dFormat, &dProps);

        if (dProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            mDepthFormat = dFormat;
            break;
        }
    }

    VkExtent2D swapchainSize;

    if (surfaceProperties.currentExtent.width == 0xFFFFFFFF) {
        swapchainSize.width = mWidth;
        swapchainSize.height = mHeight;
    } else {
        swapchainSize = surfaceProperties.currentExtent;
        mHeight = swapchainSize.height;
        mWidth = swapchainSize.width;
    }

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t desiredSwapchainImages = surfaceProperties.minImageCount + 1;

    if ((surfaceProperties.maxImageCount > 0)
        && desiredSwapchainImages > surfaceProperties.maxImageCount)
        desiredSwapchainImages = surfaceProperties.maxImageCount;

    VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    VkSwapchainCreateInfoKHR swapchainInfo{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchainInfo.surface = mSurface;
    swapchainInfo.minImageCount = desiredSwapchainImages;
    swapchainInfo.imageFormat = format.format;
    swapchainInfo.imageColorSpace = format.colorSpace;
    swapchainInfo.imageExtent.width = swapchainSize.width;
    swapchainInfo.imageExtent.height = swapchainSize.height;
    swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                               | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.compositeAlpha = composite;
    swapchainInfo.presentMode = swapchainPresentMode;
    swapchainInfo.clipped = true;

    vkCreateSwapchainKHR(mDriver->mActiveDevice, &swapchainInfo, nullptr, &vSwapchain.mSwapchain);
}
