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
    vSwapchain.mDriver = mDriver;

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
                             [this](VkSurfaceFormatKHR currFormat) {
                                 return currFormat.format == mColorFormat;
                             });

    if (iter == supportedFormatList.end()) {
        iter = supportedFormatList.begin();
        MiniEngine::Logger::wprint("{} not found as a supported format. Defaulting to {}",
                                   mColorFormat,
                                   (*iter).format);
    }

    auto format = *iter;

    vSwapchain.mColorFormat = format.format;

    // Choose desired depth as well
    constexpr VkFormat depthList[] = {VK_FORMAT_D32_SFLOAT,
                                      VK_FORMAT_D32_SFLOAT_S8_UINT,
                                      VK_FORMAT_D24_UNORM_S8_UINT};

    vSwapchain.mDepthFormat = depthList[0];

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
    swapchainInfo.presentMode = mPresentMode;
    swapchainInfo.clipped = true;

    vkCreateSwapchainKHR(mDriver->mActiveDevice, &swapchainInfo, nullptr, &vSwapchain.mSwapchain);

    vSwapchain.createPerFrameData();

    return vSwapchain;
}

void MiniEngine::Backend::VulkanSwapchain::createPerFrameData()
{
    vkGetSwapchainImagesKHR(mDriver->mActiveDevice, this->getSwapchain(), &mSwapchainCount, nullptr);

    std::vector<VkImage> swapchainImages(mSwapchainCount);
    vkGetSwapchainImagesKHR(mDriver->mActiveDevice,
                            this->getSwapchain(),
                            &mSwapchainCount,
                            swapchainImages.data());

    mSwapchainPerFrameData = std::vector<PerFrameData>(mSwapchainCount, PerFrameData{});

    // Initialize a command pool per swapchain image
    for (uint32_t i = 0; i < mSwapchainCount; ++i) {
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
        VkImageView imageView;

        VkCommandPoolCreateInfo cmdPoolInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        cmdPoolInfo.queueFamilyIndex = mDriver->mActiveQueue;
        vkCreateCommandPool(mDriver->mActiveDevice, &cmdPoolInfo, nullptr, &commandPool);

        VkCommandBufferAllocateInfo cmdBuffInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        cmdBuffInfo.commandPool = commandPool;
        cmdBuffInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBuffInfo.commandBufferCount = 1;
        vkAllocateCommandBuffers(mDriver->mActiveDevice, &cmdBuffInfo, &commandBuffer);

        mSwapchainPerFrameData[i].imageCommandPool = commandPool;
        mSwapchainPerFrameData[i].imageCommandBuffer = commandBuffer;

        VkImageViewCreateInfo viewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = this->getFormat();
        viewInfo.image = swapchainImages[i];
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

        vkCreateImageView(mDriver->mActiveDevice, &viewInfo, nullptr, &imageView);
        mSwapchainPerFrameData[i].imageView = imageView;
        mSwapchainPerFrameData[i].rawImage = swapchainImages[i];
    }
}
