#include "VulkanDriver.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{

	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
		MiniEngine::Logger::print("MiniVkVerbose: {}", pCallbackData->pMessage);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
		MiniEngine::Logger::print("MiniVkInfo: {}", pCallbackData->pMessage);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		MiniEngine::Logger::wprint("MiniVkWarning: {}", pCallbackData->pMessage);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		MiniEngine::Logger::eprint("MiniVkError: {}", pCallbackData->pMessage);
	}
	else
		MiniEngine::Logger::eprint("UnknownVkError: {}", pCallbackData->pMessage);

	return VK_FALSE;
}

void MiniEngine::Backend::VulkanDriver::initialize(MiniEngine::Types::EngineInitParams& params)
{
	mParams = params;

	VK_CHECK(volkInitialize());

	enumerateInstanceExtensionProperties();
	enumerateInstanceLayerProperties();
}

void MiniEngine::Backend::VulkanDriver::generateDevice()
{
	getPhysicalDevice();
	getPhysicalDeviceQueueFamily();
	enumerateDeviceExtensionProperties();
	createDevice({ "VK_KHR_swapchain" });
}

void MiniEngine::Backend::VulkanDriver::generateSwapchain()
{
	createSwapchain();
}

void MiniEngine::Backend::VulkanDriver::createInstance(
	const std::vector<const char*>& requireInstanceExtensions,
	const std::vector<const char*>&& requiredLayers)
{
	// TODO: Check if layers and extensions are available

	VkApplicationInfo app{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	app.pApplicationName = "MiniEngine";
	app.pEngineName = "MiniEngine";

	VkInstanceCreateInfo info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	info.pApplicationInfo = &app;
	info.enabledExtensionCount = static_cast<uint32_t>(requireInstanceExtensions.size());
	info.ppEnabledExtensionNames = requireInstanceExtensions.data();
	info.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
	info.ppEnabledLayerNames = requiredLayers.data();

#ifdef GRAPHICS_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	debugInfo.pfnUserCallback = debugUtilsCallback;

	info.pNext = &debugInfo;
#endif

	VK_CHECK(vkCreateInstance(&info, nullptr, &mInstance));

	volkLoadInstance(mInstance);

#ifdef GRAPHICS_DEBUG
	VkDebugUtilsMessengerEXT messenger;
	VK_CHECK(vkCreateDebugUtilsMessengerEXT(mInstance, &debugInfo, nullptr, &messenger));
#endif
}


void MiniEngine::Backend::VulkanDriver::enumerateInstanceExtensionProperties()
{
	uint32_t instanceExtensionCount;

	VK_CHECK( vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr) );
	std::vector<VkExtensionProperties> instanceExtensionList(instanceExtensionCount);
	VK_CHECK( vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensionList.data()) );

	for (auto i : instanceExtensionList)
	{
		MiniEngine::Logger::print(i.extensionName);
	}
}

void MiniEngine::Backend::VulkanDriver::enumerateInstanceLayerProperties()
{
	uint32_t instanceLayerCount;

	VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr));
	std::vector<VkLayerProperties> layerPropertyList(instanceLayerCount);
	VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerPropertyList.data()));

	for (auto i : layerPropertyList)
	{
		MiniEngine::Logger::print(i.layerName);
	}
}

void MiniEngine::Backend::VulkanDriver::enumerateDeviceExtensionProperties()
{
	uint32_t deviceExtensionCount;

	VK_CHECK( vkEnumerateDeviceExtensionProperties(mActiveGpu, nullptr, &deviceExtensionCount, nullptr) );
	std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);

	VK_CHECK( vkEnumerateDeviceExtensionProperties(mActiveGpu, nullptr, &deviceExtensionCount, deviceExtensions.data()) );

	// Check required extensions here (swapchain)

	MiniEngine::Logger::print("Device Extensions: ");

	for (auto i : deviceExtensions)
	{
		MiniEngine::Logger::print(i.extensionName);
	}
}

void MiniEngine::Backend::VulkanDriver::getPhysicalDevice()
{
	uint32_t gpuCount;

	VK_CHECK(vkEnumeratePhysicalDevices(mInstance, &gpuCount, nullptr));

	if (gpuCount < 1) { MiniEngine::Logger::eprint("No Vulkan device found"); throw; }

	std::vector<VkPhysicalDevice> gpuList(gpuCount);
	VK_CHECK(vkEnumeratePhysicalDevices(mInstance, &gpuCount, gpuList.data()));

	// Only taking the first GPU found
	mActiveGpu = gpuList[0];
}

void MiniEngine::Backend::VulkanDriver::getPhysicalDeviceQueueFamily()
{
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(mActiveGpu, &queueFamilyCount, nullptr);

	if(queueFamilyCount < 1) { MiniEngine::Logger::eprint("No queue family found"); throw; }

	std::vector<VkQueueFamilyProperties> queuePropertyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mActiveGpu, &queueFamilyCount, queuePropertyList.data());

	for (uint32_t i = 0; i < queueFamilyCount; ++i)
	{
		VkBool32 supportsPresent;
		vkGetPhysicalDeviceSurfaceSupportKHR(mActiveGpu, i, mSurface, &supportsPresent);

		if ((queuePropertyList[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && supportsPresent)
		{
			mActiveQueue = i;
			break;
		}
	}

	if (mActiveQueue < 0)
	{
		MiniEngine::Logger::eprint("VkError: No queue found on device that can present");
		throw;
	}
	else
		MiniEngine::Logger::print("Presenting queue found at index {}", mActiveQueue);
}

void MiniEngine::Backend::VulkanDriver::createDevice(const std::vector<const char*>&& requiredExtensions)
{
	auto qPriority = 1.0f;

	VkDeviceQueueCreateInfo queueInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	queueInfo.queueFamilyIndex = mActiveQueue;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = &qPriority;

	VkDeviceCreateInfo deviceInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	deviceInfo.ppEnabledExtensionNames = requiredExtensions.data();

	VK_CHECK( vkCreateDevice(mActiveGpu, &deviceInfo, nullptr, &mActiveDevice) );
	volkLoadDevice(mActiveDevice);

	vkGetDeviceQueue(mActiveDevice, mActiveQueue, 0, &mActiveDeviceQueue);
}

void MiniEngine::Backend::VulkanDriver::createSwapchain()
{
	VkSurfaceCapabilitiesKHR surfaceProperties;
	VK_CHECK( vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mActiveGpu, mSurface, &surfaceProperties) );

	// Preferred format VK_FORMAT_R16G16B16A16_SFLOAT
	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(mActiveGpu, mSurface, &surfaceFormatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> supportedFormatList(surfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(mActiveGpu, mSurface, &surfaceFormatCount, supportedFormatList.data());

	auto iter = std::find_if(supportedFormatList.begin(), supportedFormatList.end(),
		[](VkSurfaceFormatKHR currFormat) { return currFormat.format == VK_FORMAT_R16G16B16A16_SFLOAT; });

	if (iter == supportedFormatList.end())
	{
		MiniEngine::Logger::eprint("VK_FORMAT_R16G16B16A16_SFLOAT not found as a supported format");
		throw;
	}

	auto format = *iter;

	VkExtent2D swapchainSize;

	if (surfaceProperties.currentExtent.width == 0xFFFFFFFF)
	{
		swapchainSize.width = mParams.screenWidth;
		swapchainSize.height = mParams.screenHeight;
	}
	else
	{
		swapchainSize = surfaceProperties.currentExtent;
	}

	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	uint32_t desiredSwapchainImages = surfaceProperties.minImageCount + 1;

	if ((surfaceProperties.maxImageCount > 0) && desiredSwapchainImages > surfaceProperties.maxImageCount)
		desiredSwapchainImages = surfaceProperties.maxImageCount;

	VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	VkSwapchainCreateInfoKHR swapchainInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	swapchainInfo.surface = mSurface;
	swapchainInfo.minImageCount = desiredSwapchainImages;
	swapchainInfo.imageFormat = format.format;
	swapchainInfo.imageColorSpace = format.colorSpace;
	swapchainInfo.imageExtent.width = swapchainSize.width;
	swapchainInfo.imageExtent.height = swapchainSize.height;
	swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.compositeAlpha = composite;
	swapchainInfo.presentMode = swapchainPresentMode;
	swapchainInfo.clipped = true;

	VK_CHECK( vkCreateSwapchainKHR(mActiveDevice, &swapchainInfo, nullptr, &mActiveSwapchain) );
}

void MiniEngine::Backend::VulkanDriver::createSwapchainImageViews()
{
	uint32_t imageCount;
	VK_CHECK( vkGetSwapchainImagesKHR(mActiveDevice, mActiveSwapchain, &imageCount, nullptr) );

	std::vector<VkImage> swapchainImages(imageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(mActiveDevice, mActiveSwapchain, &imageCount, swapchainImages.data()));

	// Initialize per frame
	for (uint32_t i = 0; i < imageCount; ++i)
	{
		VkFence fence;

		VkFenceCreateInfo imageFenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		imageFenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VK_CHECK( vkCreateFence(mActiveDevice, &imageFenceInfo, nullptr, &fence) );

		VkCommandPoolCreateInfo cmdPoolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	}
}

unsigned int MiniEngine::Backend::VulkanDriver::createTexture(int width, int height, int channels, void* data, Texture::TextureType type)
{
	return 0;
}

unsigned int MiniEngine::Backend::VulkanDriver::createUniformBlock(size_t dataSize, unsigned int bindIndex) const
{
	return 0;
}

void MiniEngine::Backend::VulkanDriver::updateUniformData(unsigned int bufferId, unsigned int offset, size_t size, void* data) const
{
}

void MiniEngine::Backend::VulkanDriver::registerUniformBlock(const char* blockName, const Shader* program, unsigned int layoutIndex) const
{
}

void MiniEngine::Backend::VulkanDriver::setupMesh(MiniEngine::Components::RenderableComponent* component)
{
}

void MiniEngine::Backend::VulkanDriver::setupSkybox(MiniEngine::Components::SkyboxComponent* skybox)
{
}

unsigned int MiniEngine::Backend::VulkanDriver::loadShader(const char* path, ShaderType type) const
{
	return 0;
}

unsigned int MiniEngine::Backend::VulkanDriver::createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const
{
	return 0;
}

void MiniEngine::Backend::VulkanDriver::useShaderProgram(unsigned int program) const
{
}

void MiniEngine::Backend::VulkanDriver::setFloat(unsigned int program, const char* name, float value) const
{
}

void MiniEngine::Backend::VulkanDriver::setVec3(unsigned int program, const char* name, Vector3 value) const
{
}

void MiniEngine::Backend::VulkanDriver::setMat4(unsigned int program, const char* name, Matrix4x4 value) const
{
}
