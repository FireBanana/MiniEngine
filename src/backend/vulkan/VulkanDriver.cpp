#include "VulkanDriver.h"
#include "GlslCompiler.h"
#include "VulkanRenderDoc.h"
#include "VulkanPipelineBuilder.h"

#define RESOLVE_PATH(path) DIR##path

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	return VK_FALSE;
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
		MiniEngine::Logger::print("MiniVkVerbose: {}\n", pCallbackData->pMessage);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
		MiniEngine::Logger::print("MiniVkInfo: {}\n", pCallbackData->pMessage);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		MiniEngine::Logger::wprint("MiniVkWarning: {}\n", pCallbackData->pMessage);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		MiniEngine::Logger::eprint("MiniVkError: {}\n", pCallbackData->pMessage);
	}
	else
		MiniEngine::Logger::eprint("UnknownVkError: {}\n", pCallbackData->pMessage);

	return VK_FALSE;
}

void MiniEngine::Backend::VulkanDriver::initialize(MiniEngine::Types::EngineInitParams& params)
{
	mParams = params;

	volkInitialize();

	enumerateInstanceExtensionProperties();
	enumerateInstanceLayerProperties();
}

MiniEngine::Backend::VulkanDriver::~VulkanDriver()
{
	delete mPipelineBuilder;
}

void MiniEngine::Backend::VulkanDriver::generateDevice()
{
    registerPhysicalDevice();
    registerPhysicalDeviceQueueFamily();
    enumerateDeviceExtensionProperties();
    createDevice({"VK_KHR_swapchain",
                  "VK_KHR_dynamic_rendering",
                  "VK_KHR_depth_stencil_resolve",
                  "VK_KHR_create_renderpass2",
                  "VK_KHR_multiview",
                  "VK_KHR_maintenance2",
				  "VK_KHR_dynamic_rendering_local_read"});
}

void MiniEngine::Backend::VulkanDriver::generateSwapchain()
{
	createSwapchain();
	createSwapchainImageViews();
	createDisplaySemaphores();
}

void MiniEngine::Backend::VulkanDriver::generatePipelines()
{
	mPipelineBuilder = new VulkanPipelineBuilder{ this };

	mPipelineBuilder->instantiateTriangleBuffer();
	mPipelineBuilder->createDefaultPipelineLayout(mDefaultPipelineLayout);

	createGBufferPipeline();
	createLightingPipeline();

	mPipelineBuilder->updateSceneDescriptorSetData();

	recordCommandBuffers();
}

void MiniEngine::Backend::VulkanDriver::generateGbuffer()
{
	auto colorImageView = createImageAttachment(mCurrentSwapchainFormat,
		VK_IMAGE_USAGE_SAMPLED_BIT
		| VK_IMAGE_USAGE_STORAGE_BIT
		| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT,
		"Color Target");
	auto positionImageView = createImageAttachment(mCurrentSwapchainFormat,
		VK_IMAGE_USAGE_SAMPLED_BIT
		| VK_IMAGE_USAGE_STORAGE_BIT
		| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT,
		"Position Target");
	auto normalImageView = createImageAttachment(mCurrentSwapchainFormat,
		VK_IMAGE_USAGE_SAMPLED_BIT
		| VK_IMAGE_USAGE_STORAGE_BIT
		| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT,
		"Normals Target");
	auto roughnessImageView = createImageAttachment(mCurrentSwapchainFormat,
		VK_IMAGE_USAGE_SAMPLED_BIT
		| VK_IMAGE_USAGE_STORAGE_BIT
		| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT,
		"Roughness Target");
	auto depthImageView = createImageAttachment(mCurrentSwapchainDepthFormat,
		VK_IMAGE_USAGE_SAMPLED_BIT
		| VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		"Depth Target");

	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::COLOR)] = colorImageView;
	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::POSITION)] = positionImageView;
	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::NORMAL)] = normalImageView;
	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::ROUGHNESS)] = roughnessImageView;
	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::DEPTH)] = depthImageView;
}

void MiniEngine::Backend::VulkanDriver::createInstance(
	const std::vector<const char*>& requireInstanceExtensions,
	const std::vector<const char*>&& requiredLayers)
{
	// TODO: Check if layers and extensions are available

	VkApplicationInfo app{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	app.pApplicationName = "MiniEngine";
	app.pEngineName = "MiniEngine";
	app.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 261);

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

	vkCreateInstance(&info, nullptr, &mInstance);

	volkLoadInstance(mInstance);

#ifdef GRAPHICS_DEBUG
	VkDebugUtilsMessengerEXT messenger;
	vkCreateDebugUtilsMessengerEXT(mInstance, &debugInfo, nullptr, &messenger);
#endif
}


void MiniEngine::Backend::VulkanDriver::enumerateInstanceExtensionProperties()
{
	uint32_t instanceExtensionCount;

	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
	std::vector<VkExtensionProperties> instanceExtensionList(instanceExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr,
		&instanceExtensionCount,
		instanceExtensionList.data());

	for (auto i : instanceExtensionList) {
		MiniEngine::Logger::print(i.extensionName);
	}
}

void MiniEngine::Backend::VulkanDriver::enumerateInstanceLayerProperties()
{
	uint32_t instanceLayerCount;

	vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
	std::vector<VkLayerProperties> layerPropertyList(instanceLayerCount);
	vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerPropertyList.data());

	for (auto i : layerPropertyList) {
		MiniEngine::Logger::print(i.layerName);
	}
}

void MiniEngine::Backend::VulkanDriver::enumerateDeviceExtensionProperties()
{
	uint32_t deviceExtensionCount;

	vkEnumerateDeviceExtensionProperties(mActiveGpu, nullptr, &deviceExtensionCount, nullptr);
	std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);

	vkEnumerateDeviceExtensionProperties(mActiveGpu,
		nullptr,
		&deviceExtensionCount,
		deviceExtensions.data());

	// Check required extensions here (swapchain)

	MiniEngine::Logger::print("Device Extensions: ");

	for (auto i : deviceExtensions) {
		MiniEngine::Logger::print(i.extensionName);
	}
}

void MiniEngine::Backend::VulkanDriver::registerPhysicalDevice()
{
	uint32_t gpuCount;

	vkEnumeratePhysicalDevices(mInstance, &gpuCount, nullptr);

	if (gpuCount < 1) {
		MiniEngine::Logger::eprint("No Vulkan device found");
		throw;
	}

	std::vector<VkPhysicalDevice> gpuList(gpuCount);
	vkEnumeratePhysicalDevices(mInstance, &gpuCount, gpuList.data());

	// Only taking the first GPU found
	mActiveGpu = gpuList.size() > 1 ? gpuList[1] : gpuList[0];

	vkGetPhysicalDeviceMemoryProperties(mActiveGpu, &mGpuMemoryProperties);
}

void MiniEngine::Backend::VulkanDriver::registerPhysicalDeviceQueueFamily()
{
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(mActiveGpu, &queueFamilyCount, nullptr);

	if (queueFamilyCount < 1) { MiniEngine::Logger::eprint("No queue family found"); throw; }

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
	const float qPriority[] = { 1.0f };

	VkDeviceQueueCreateInfo queueInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	queueInfo.queueFamilyIndex = mActiveQueue;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = qPriority;

	VkPhysicalDeviceDynamicRenderingFeatures dynamicInfo{
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };
	dynamicInfo.dynamicRendering = true;

	// Enable storage image write feature
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(mActiveGpu, &features);
	features.fragmentStoresAndAtomics = VK_TRUE;

	VkDeviceCreateInfo deviceInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	deviceInfo.ppEnabledExtensionNames = requiredExtensions.data();
	deviceInfo.pEnabledFeatures = &features;
	deviceInfo.pNext = &dynamicInfo;

	vkCreateDevice(mActiveGpu, &deviceInfo, nullptr, &mActiveDevice);
	volkLoadDevice(mActiveDevice);

	// Getting first queue only
	vkGetDeviceQueue(mActiveDevice, mActiveQueue, 0, &mActiveDeviceQueue);

	VkPhysicalDeviceProperties deviceProps;
	vkGetPhysicalDeviceProperties(mActiveGpu, &deviceProps);

	MiniEngine::Logger::print("Device Name: {}, Driver Version: {}",
		deviceProps.deviceName,
		deviceProps.driverVersion);
}

void MiniEngine::Backend::VulkanDriver::createSwapchain()
{
	VkSurfaceCapabilitiesKHR surfaceProperties;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mActiveGpu, mSurface, &surfaceProperties);

	// Preferred format VK_FORMAT_R16G16B16A16_SFLOAT
	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(mActiveGpu, mSurface, &surfaceFormatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> supportedFormatList(surfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(mActiveGpu,
		mSurface,
		&surfaceFormatCount,
		supportedFormatList.data());

	auto iter = std::find_if(supportedFormatList.begin(),
		supportedFormatList.end(),
		[](VkSurfaceFormatKHR currFormat) {
			return currFormat.format == PREFERRED_FORMAT;
		});

	if (iter == supportedFormatList.end()) {
		iter = supportedFormatList.begin();
		MiniEngine::Logger::wprint("{} not found as a supported format. Defaulting to {}",
			PREFERRED_FORMAT,
			(*iter).format);
	}

	auto format = *iter;

	mCurrentSwapchainFormat = format.format;

	// Choose desired depth as well
	constexpr VkFormat depthList[] = { VK_FORMAT_D32_SFLOAT,
									  VK_FORMAT_D32_SFLOAT_S8_UINT,
									  VK_FORMAT_D24_UNORM_S8_UINT };

	mCurrentSwapchainDepthFormat = depthList[0];

	for (auto dFormat : depthList) {
		VkFormatProperties dProps;
		vkGetPhysicalDeviceFormatProperties(mActiveGpu, dFormat, &dProps);

		if (dProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			mCurrentSwapchainDepthFormat = dFormat;
			break;
		}
	}

	VkExtent2D swapchainSize;

	if (surfaceProperties.currentExtent.width == 0xFFFFFFFF) {
		swapchainSize.width = mParams.screenWidth;
		swapchainSize.height = mParams.screenHeight;
	}
	else {
		swapchainSize = surfaceProperties.currentExtent;
		mParams.screenHeight = swapchainSize.height;
		mParams.screenWidth = swapchainSize.width;
	}

	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	uint32_t desiredSwapchainImages = surfaceProperties.minImageCount + 1;

	if ((surfaceProperties.maxImageCount > 0)
		&& desiredSwapchainImages > surfaceProperties.maxImageCount)
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
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.compositeAlpha = composite;
	swapchainInfo.presentMode = swapchainPresentMode;
	swapchainInfo.clipped = true;

	vkCreateSwapchainKHR(mActiveDevice, &swapchainInfo, nullptr, &mActiveSwapchain);
}

void MiniEngine::Backend::VulkanDriver::createSwapchainImageViews()
{
	uint32_t imageCount;
	vkGetSwapchainImagesKHR(mActiveDevice, mActiveSwapchain, &imageCount, nullptr);

	std::vector<VkImage> swapchainImages(imageCount);
	vkGetSwapchainImagesKHR(mActiveDevice, mActiveSwapchain, &imageCount, swapchainImages.data());

	mSwapchainPerImageData = std::vector<PerFrameData>(imageCount, PerFrameData{});

	// Initialize a command pool per swapchain image
	for (uint32_t i = 0; i < imageCount; ++i) {
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
		VkImageView imageView;

		VkCommandPoolCreateInfo cmdPoolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		cmdPoolInfo.queueFamilyIndex = mActiveQueue;
		vkCreateCommandPool(mActiveDevice, &cmdPoolInfo, nullptr, &commandPool);

		VkCommandBufferAllocateInfo cmdBuffInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		cmdBuffInfo.commandPool = commandPool;
		cmdBuffInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBuffInfo.commandBufferCount = 1;
		vkAllocateCommandBuffers(mActiveDevice, &cmdBuffInfo, &commandBuffer);

		mSwapchainPerImageData[i].imageCommandPool = commandPool;
		mSwapchainPerImageData[i].imageCommandBuffer = commandBuffer;

		VkImageViewCreateInfo viewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = mCurrentSwapchainFormat;
		viewInfo.image = swapchainImages[i];
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

		vkCreateImageView(mActiveDevice, &viewInfo, nullptr, &imageView);
		mSwapchainPerImageData[i].imageView = imageView;

		mSwapchainPerImageData[i].rawImage = swapchainImages[i];
	}
}

void MiniEngine::Backend::VulkanDriver::createGBufferPipeline()
{
	auto vertexInputInfo = mPipelineBuilder->createTriangleVertexInputState();
	auto inputAssemblyInfo = mPipelineBuilder->createDefaultInputAssemblyState();
	auto rasterInfo = mPipelineBuilder->createDefaultRasterState();
	auto viewportInfo = mPipelineBuilder->createDefaultPipelineViewportState();
	auto multiSampleInfo = mPipelineBuilder->createDefaultPipelineMultisampleState();
	auto shaderStages = mPipelineBuilder->createDefaultVertFragShaderStage(RESOLVE_PATH("/shaders/temp.vert"), RESOLVE_PATH("/shaders/temp.frag"));

	// address of array changes so need to assign in scope
	vertexInputInfo.data.pVertexAttributeDescriptions = vertexInputInfo.vertexAttributeDescription.data();
	vertexInputInfo.data.pVertexBindingDescriptions = vertexInputInfo.vertexBindingDescriptions.data();

	VkPipelineColorBlendAttachmentState colorBlendState{};
	colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendAttachmentState blendList[] = { colorBlendState, colorBlendState, colorBlendState, colorBlendState };

	VkPipelineColorBlendStateCreateInfo colorBlendInfo{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	colorBlendInfo.attachmentCount = 4;
	colorBlendInfo.pAttachments = blendList;

	VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	depthStencilInfo.depthTestEnable = true;
	depthStencilInfo.depthWriteEnable = true;
	depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;

	// Change to static
	std::array<VkDynamicState, 2> dynamics{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicInfo{
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamicInfo.pDynamicStates = dynamics.data();
	dynamicInfo.dynamicStateCount = static_cast<uint32_t>(dynamics.size());

	VkFormat colorAttachmentFormats[] = { mCurrentSwapchainFormat,
										 mCurrentSwapchainFormat,
										 mCurrentSwapchainFormat,
										 mCurrentSwapchainFormat };

	VkPipelineRenderingCreateInfo renderingInfo{ VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	renderingInfo.colorAttachmentCount = 4;
	renderingInfo.pColorAttachmentFormats = colorAttachmentFormats;
	renderingInfo.depthAttachmentFormat = mCurrentSwapchainDepthFormat;

	VkGraphicsPipelineCreateInfo pipelineInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &(vertexInputInfo.data);
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pRasterizationState = &rasterInfo;
	pipelineInfo.pColorBlendState = &colorBlendInfo;
	pipelineInfo.pMultisampleState = &multiSampleInfo;
	pipelineInfo.pViewportState = &viewportInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.pDynamicState = &dynamicInfo;
	pipelineInfo.layout = mDefaultPipelineLayout;
	pipelineInfo.pNext = &renderingInfo;

	vkCreateGraphicsPipelines(mActiveDevice,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&mGbufferPipeline);

	vkDestroyShaderModule(mActiveDevice, shaderStages[0].module, nullptr);
	vkDestroyShaderModule(mActiveDevice, shaderStages[1].module, nullptr);
}

void MiniEngine::Backend::VulkanDriver::createLightingPipeline()
{
	auto vertexInputInfo = mPipelineBuilder->createTriangleVertexInputState();
	auto inputAssemblyInfo = mPipelineBuilder->createDefaultInputAssemblyState();
	auto rasterInfo = mPipelineBuilder->createDefaultRasterState();
	auto viewportInfo = mPipelineBuilder->createDefaultPipelineViewportState();
	auto multiSampleInfo = mPipelineBuilder->createDefaultPipelineMultisampleState();
	auto shaderStages = mPipelineBuilder->createDefaultVertFragShaderStage(
		RESOLVE_PATH("/shaders/lighting_vk.vert"), RESOLVE_PATH("/shaders/lighting_vk.frag"));

	// address of array changes so need to assign in scope
	vertexInputInfo.data.pVertexAttributeDescriptions = vertexInputInfo.vertexAttributeDescription.data();
	vertexInputInfo.data.pVertexBindingDescriptions = vertexInputInfo.vertexBindingDescriptions.data();

	VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	depthStencilInfo.depthTestEnable = false;
	depthStencilInfo.depthWriteEnable = false;

	std::array<VkDynamicState, 2> dynamics{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicInfo{
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamicInfo.pDynamicStates = dynamics.data();
	dynamicInfo.dynamicStateCount = static_cast<uint32_t>(dynamics.size());

	VkPipelineColorBlendAttachmentState colorBlendState{};
	colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendAttachmentState blendList[] = { colorBlendState, colorBlendState , colorBlendState , colorBlendState, colorBlendState };

	VkPipelineColorBlendStateCreateInfo colorBlendInfo{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	colorBlendInfo.attachmentCount = 5;
	colorBlendInfo.pAttachments = blendList;

	VkFormat colorAttachmentFormats[] = { mCurrentSwapchainFormat,
										 mCurrentSwapchainFormat,
										 mCurrentSwapchainFormat,
										 mCurrentSwapchainFormat,
										 mCurrentSwapchainFormat };

	VkPipelineRenderingCreateInfo renderingInfo{ VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	renderingInfo.colorAttachmentCount = 5;
	renderingInfo.pColorAttachmentFormats = colorAttachmentFormats;
	renderingInfo.depthAttachmentFormat = mCurrentSwapchainDepthFormat;

	VkGraphicsPipelineCreateInfo pipelineInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo.data;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pRasterizationState = &rasterInfo;
	pipelineInfo.pColorBlendState = &colorBlendInfo;
	pipelineInfo.pMultisampleState = &multiSampleInfo;
	pipelineInfo.pViewportState = &viewportInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.pDynamicState = &dynamicInfo;
	pipelineInfo.layout = mDefaultPipelineLayout;
	pipelineInfo.pNext = &renderingInfo;

	vkCreateGraphicsPipelines(mActiveDevice,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&mLightingPipeline);

	vkDestroyShaderModule(mActiveDevice, shaderStages[0].module, nullptr);
	vkDestroyShaderModule(mActiveDevice, shaderStages[1].module, nullptr);
}

void MiniEngine::Backend::VulkanDriver::createDisplaySemaphores()
{
	mDisplaySemaphoreArray = Utils::DynamicArray<DisplaySemaphore>(mSwapchainPerImageData.size());

	auto arrayPtr = mDisplaySemaphoreArray.get();

	VkSemaphoreCreateInfo info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	fInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < mSwapchainPerImageData.size(); ++i) {
		vkCreateSemaphore(mActiveDevice, &info, nullptr, &(arrayPtr[i].acquisitionSemaphore));
		vkCreateSemaphore(mActiveDevice, &info, nullptr, &(arrayPtr[i].presentationSemaphore));
		vkCreateFence(mActiveDevice, &fInfo, nullptr, &(arrayPtr[i].fence));
	}
}

void MiniEngine::Backend::VulkanDriver::recordCommandBuffers()
{
	VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };

	for (auto i = 0; i < mSwapchainPerImageData.size(); ++i)
	{
		auto& cmd = mSwapchainPerImageData[i].imageCommandBuffer;

		vkBeginCommandBuffer(cmd, &beginInfo);

		auto swapchainImage = mSwapchainPerImageData[i].rawImage;
		auto colorImage = mImageAttachments[static_cast<int>(ImageAttachmentType::COLOR)].rawImage;
		auto positionImage = mImageAttachments[static_cast<int>(ImageAttachmentType::POSITION)].rawImage;
		auto normalImage = mImageAttachments[static_cast<int>(ImageAttachmentType::NORMAL)].rawImage;
		auto roughnessImage = mImageAttachments[static_cast<int>(ImageAttachmentType::ROUGHNESS)].rawImage;

		VkRenderingAttachmentInfo gBufferColorAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		gBufferColorAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::COLOR)].imageView;
		gBufferColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		gBufferColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferColorAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo gBufferPositionAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		gBufferPositionAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::POSITION)].imageView;
		gBufferPositionAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		gBufferPositionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferPositionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferPositionAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo gBufferNormalAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		gBufferNormalAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::NORMAL)].imageView;
		gBufferNormalAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		gBufferNormalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferNormalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferNormalAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo gBufferRoughnessAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		gBufferRoughnessAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::ROUGHNESS)].imageView;
		gBufferRoughnessAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		gBufferRoughnessAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferRoughnessAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferRoughnessAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo depthAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		depthAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::DEPTH)].imageView;
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.clearValue.depthStencil = { 1, 0 };

		VkRenderingAttachmentInfo gBufferAttachmentArray[] = { gBufferColorAttachment,
													   gBufferPositionAttachment,
													   gBufferNormalAttachment,
													   gBufferRoughnessAttachment };

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mGbufferPipeline);

		VkViewport vp{};
		vp.width = mParams.screenWidth;
		vp.height = mParams.screenHeight;
		vp.minDepth = 0.0f;
		vp.maxDepth = 1.0f;

		vkCmdSetViewport(cmd, 0, 1, &vp);

		VkRect2D scissor{};
		scissor.extent.width = mParams.screenWidth;
		scissor.extent.height = mParams.screenHeight;

        auto tBuffer = mPipelineBuilder->getDefaultTriangleBuffer().getRawBuffer();
        VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(cmd, 0, 1, &tBuffer, offsets);

		vkCmdSetScissor(cmd, 0, 1, &scissor);

		VkRenderingInfoKHR gBufferRenderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
		gBufferRenderingInfo.layerCount = 1;
		gBufferRenderingInfo.colorAttachmentCount = 4;
		gBufferRenderingInfo.pColorAttachments = gBufferAttachmentArray;
		gBufferRenderingInfo.pDepthAttachment = &depthAttachment;
		gBufferRenderingInfo.renderArea = { 0, 0, mParams.screenWidth, mParams.screenHeight };

		createPipelineBarrier(swapchainImage,
			cmd,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

		// TODO: Differing behavior on AMD vs Nvidia, revisit
		createPipelineBarrier(colorImage,
			cmd,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

		createPipelineBarrier(positionImage,
			cmd,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

		createPipelineBarrier(normalImage,
			cmd,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

		createPipelineBarrier(roughnessImage,
			cmd,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

		auto descriptorSet = mPipelineBuilder->getSceneBlockDescriptorSet();
		vkCmdBindDescriptorSets(cmd,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			mDefaultPipelineLayout,
			0,
			1,
			&descriptorSet,
			0,
			nullptr);

		// GBuffer pass
		vkCmdBeginRendering(cmd, &gBufferRenderingInfo);
		vkCmdDraw(cmd, 3, 1, 0, 0);
		vkCmdEndRendering(cmd);

		// ================ LIGHTING ==================================================================================

		VkRenderingAttachmentInfo lightingSwapChainAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingSwapChainAttachment.imageView = mSwapchainPerImageData[i].imageView;
		lightingSwapChainAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingSwapChainAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		lightingSwapChainAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		lightingSwapChainAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo lightingColorAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingColorAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::COLOR)].imageView;
		lightingColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		lightingColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		lightingColorAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo lightingPositionAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingPositionAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::POSITION)].imageView;
		lightingPositionAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingPositionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		lightingPositionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		lightingPositionAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo lightingNormalAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingNormalAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::NORMAL)].imageView;
		lightingNormalAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingNormalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		lightingNormalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		lightingNormalAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo lightingRoughnessAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingRoughnessAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::ROUGHNESS)].imageView;
		lightingRoughnessAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingRoughnessAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		lightingRoughnessAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		lightingRoughnessAttachment.clearValue.color = { 1, 0, 0, 1 };

		auto lightingAttachmentArray = { lightingColorAttachment, lightingPositionAttachment, lightingNormalAttachment,
			lightingRoughnessAttachment, lightingSwapChainAttachment };

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mLightingPipeline);

		VkRenderingInfoKHR lightingRenderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
		lightingRenderingInfo.layerCount = 1;
		lightingRenderingInfo.colorAttachmentCount = 5;
		lightingRenderingInfo.pColorAttachments = lightingAttachmentArray.begin();
		lightingRenderingInfo.renderArea = { 0, 0, mParams.screenWidth, mParams.screenHeight };

		// Create new attachment array here with different load store
		// Lighting pass
		vkCmdBeginRendering(cmd, &lightingRenderingInfo);
		vkCmdDraw(cmd, 3, 1, 0, 0);
		vkCmdEndRendering(cmd);

		// swapchain present
		createPipelineBarrier(swapchainImage,
			cmd,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			0,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

		vkEndCommandBuffer(cmd);
	}
}

MiniEngine::Backend::VulkanDriver::ImageAttachmentData
MiniEngine::Backend::VulkanDriver::createImageAttachment(VkFormat imageFormat,
	VkImageUsageFlags imageBits,
	VkImageAspectFlags imageViewAspectFlags,
	std::string debugName)
{
	VkImageCreateInfo attachmentImageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	attachmentImageInfo.imageType = VK_IMAGE_TYPE_2D;
	attachmentImageInfo.format = imageFormat;
	attachmentImageInfo.extent.width = mParams.screenWidth;
	attachmentImageInfo.extent.height = mParams.screenHeight;
	attachmentImageInfo.extent.depth = 1;
	attachmentImageInfo.arrayLayers = 1;
	attachmentImageInfo.mipLevels = 1;
	attachmentImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	attachmentImageInfo.usage = imageBits;

	VkMemoryAllocateInfo attachmentAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	VkMemoryRequirements attachmentMemReqs;
	VkImage attachmentImage;
	VkImageView attachmentImageView;
	VkDeviceMemory attachmentMemory;

	vkCreateImage(mActiveDevice, &attachmentImageInfo, nullptr, &attachmentImage);
	vkGetImageMemoryRequirements(mActiveDevice, attachmentImage, &attachmentMemReqs);

	// Get memory info

	uint32_t index = getMemoryTypeIndex(&attachmentMemReqs);

	attachmentAllocateInfo.allocationSize = attachmentMemReqs.size;
	attachmentAllocateInfo.memoryTypeIndex = index;
	vkAllocateMemory(mActiveDevice, &attachmentAllocateInfo, nullptr, &attachmentMemory);
	vkBindImageMemory(mActiveDevice, attachmentImage, attachmentMemory, 0);

	VkImageViewCreateInfo colorImageViewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	colorImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	colorImageViewInfo.format = imageFormat;
	colorImageViewInfo.subresourceRange = {};
	colorImageViewInfo.subresourceRange.aspectMask = imageViewAspectFlags;
	colorImageViewInfo.subresourceRange.baseMipLevel = 0;
	colorImageViewInfo.subresourceRange.levelCount = 1;
	colorImageViewInfo.subresourceRange.baseArrayLayer = 0;
	colorImageViewInfo.subresourceRange.layerCount = 1;
	colorImageViewInfo.image = attachmentImage;
	vkCreateImageView(mActiveDevice, &colorImageViewInfo, nullptr, &attachmentImageView);

#ifdef GRAPHICS_DEBUG

	VkDebugUtilsObjectNameInfoEXT imageNameInfo{};
	imageNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	imageNameInfo.pNext = NULL;
	imageNameInfo.objectType = VK_OBJECT_TYPE_IMAGE_VIEW;
	imageNameInfo.objectHandle = (uint64_t)attachmentImageView;
	imageNameInfo.pObjectName = debugName.c_str();

	vkSetDebugUtilsObjectNameEXT(mActiveDevice, &imageNameInfo);

#endif

	return { attachmentImage, attachmentImageView };
}

void MiniEngine::Backend::VulkanDriver::createPipelineBarrier(VkImage image,
	VkCommandBuffer buffer,
	VkAccessFlags srcAccessMask,
	VkAccessFlags dstAccessMask,
	VkImageAspectFlags aspectMask,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask)
{
	VkImageMemoryBarrier imageMemBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	imageMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemBarrier.srcAccessMask = srcAccessMask;
	imageMemBarrier.dstAccessMask = dstAccessMask;
	imageMemBarrier.image = image;
	imageMemBarrier.subresourceRange.aspectMask = aspectMask;
	imageMemBarrier.subresourceRange.levelCount = 1;
	imageMemBarrier.subresourceRange.layerCount = 1;
	imageMemBarrier.newLayout = newLayout;
	imageMemBarrier.oldLayout = oldLayout;

	vkCmdPipelineBarrier(
		buffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemBarrier);
}

void MiniEngine::Backend::VulkanDriver::loadShaderModule() {}

void MiniEngine::Backend::VulkanDriver::acquireNextImage(uint32_t* image,
	uint32_t* displaySemaphoreIndex)
{
	std::vector<VkFence> mFenceArray(mDisplaySemaphoreArray.getSize());
	VkSemaphore dSemaphore;

	for (int i = 0; i < mDisplaySemaphoreArray.getSize(); ++i) {
		mFenceArray[i] = mDisplaySemaphoreArray.get()[i].fence;
	}

	vkWaitForFences(mActiveDevice,
		mDisplaySemaphoreArray.getSize(),
		mFenceArray.data(),
		VK_FALSE,
		UINT64_MAX);

	for (int i = 0; i < mDisplaySemaphoreArray.getSize(); ++i) {
		auto displaySyncObj = &mDisplaySemaphoreArray.get()[i];

		if (vkGetFenceStatus(mActiveDevice, displaySyncObj->fence) == VK_SUCCESS) {
			vkResetFences(mActiveDevice, 1, &displaySyncObj->fence);
			dSemaphore = displaySyncObj->acquisitionSemaphore;
			*displaySemaphoreIndex = i;
			break;
		}
	}

	auto status = vkAcquireNextImageKHR(mActiveDevice,
		mActiveSwapchain,
		UINT64_MAX,
		dSemaphore,
		VK_NULL_HANDLE,
		image);

	if (status != VK_SUCCESS)
		MiniEngine::Logger::eprint("Acquire error: {}", status);
}

uint32_t MiniEngine::Backend::VulkanDriver::getMemoryTypeIndex(const VkMemoryRequirements* memReqs)
{
	uint32_t index = 0;
	auto typeBits = memReqs->memoryTypeBits;

	for (; index < mGpuMemoryProperties.memoryTypeCount; ++index)
	{
		if ((typeBits & 1) == 1)
		{
			if ((mGpuMemoryProperties.memoryTypes[index].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				== VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				break;
		}

		typeBits >>= 1;
	}

	return index;
}

MiniEngine::Backend::VulkanBuffer MiniEngine::Backend::VulkanDriver::createBuffer(
    size_t memSize, VkBufferUsageFlags usageFlags)
{
    VulkanBuffer buffer{mActiveDevice, mGpuMemoryProperties};
    buffer.Create(memSize, usageFlags);
    return buffer;
}

// ==== Interface ====

void MiniEngine::Backend::VulkanDriver::draw(MiniEngine::Scene* scene)
{
	uint32_t imgIndex, displaySemaphoreIndex;
	acquireNextImage(&imgIndex, &displaySemaphoreIndex);

	auto& cmd = mSwapchainPerImageData[imgIndex].imageCommandBuffer;

	VkPipelineStageFlags waitStageFlags{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &(mDisplaySemaphoreArray.get()[displaySemaphoreIndex].acquisitionSemaphore);
	submitInfo.pWaitDstStageMask = &waitStageFlags;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &(mDisplaySemaphoreArray.get()[displaySemaphoreIndex].presentationSemaphore);

	vkQueueSubmit(mActiveDeviceQueue,
		1,
		&submitInfo,
		mDisplaySemaphoreArray.get()[displaySemaphoreIndex].fence);

	// Present
	VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &mActiveSwapchain;
	presentInfo.pImageIndices = &imgIndex;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &(mDisplaySemaphoreArray.get()[displaySemaphoreIndex].presentationSemaphore);

	vkQueuePresentKHR(mActiveDeviceQueue, &presentInfo);
}

unsigned int MiniEngine::Backend::VulkanDriver::createTexture(
    int width, int height, int channels, void *data, TextureType type)
{
	VkImage image;
	VkDeviceMemory mem;
	VkImageView imageView;

	VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = width;
	createInfo.extent.height = height;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

	vkCreateImage(mActiveDevice, &createInfo, nullptr, &image);

	VkMemoryAllocateInfo memAllocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	VkMemoryRequirements memReqs{};

	vkGetImageMemoryRequirements(mActiveDevice, image, &memReqs);

	memAllocInfo.allocationSize = memReqs.size;
	memAllocInfo.memoryTypeIndex = getMemoryTypeIndex(&memReqs);

	vkAllocateMemory(mActiveDevice, &memAllocInfo, nullptr, &mem);
	vkBindImageMemory(mActiveDevice, image, mem, 0);

	// Image view
	VkImageViewCreateInfo imageViewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.format = mCurrentSwapchainFormat;
	imageViewInfo.subresourceRange = {};
	imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewInfo.subresourceRange.levelCount = 1;
	imageViewInfo.subresourceRange.layerCount = 1;
	imageViewInfo.image = image;

	vkCreateImageView(mActiveDevice, &imageViewInfo, nullptr, &imageView);

	return 0; //todo: fix
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

void MiniEngine::Backend::VulkanDriver::beginRenderpass()
{

}

void MiniEngine::Backend::VulkanDriver::endRenderpass()
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
