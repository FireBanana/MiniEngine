#include "VulkanDriver.h"
#include "GlslCompiler.h"
#include "VulkanRenderDoc.h"

#define RESOLVE_PATH(path) DIR##path

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{

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
	createSwapchainImageViews();
}

void MiniEngine::Backend::VulkanDriver::generateRenderPass()
{
	createRenderPass();
}

void MiniEngine::Backend::VulkanDriver::generatePipeline()
{
	createPipeline();
	createFramebuffer();
}

void MiniEngine::Backend::VulkanDriver::generateGbuffer()
{
	auto colorImageView = createImageAttachment(mCurrentSwapchainFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	auto positionImageView = createImageAttachment(mCurrentSwapchainFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	auto normalImageView = createImageAttachment(mCurrentSwapchainFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	auto roughnessImageView = createImageAttachment(mCurrentSwapchainFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	auto depthImageView = createImageAttachment(mCurrentSwapchainDepthFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	mFrameBufferAttachments[0] = colorImageView;
	mFrameBufferAttachments[1] = positionImageView;
	mFrameBufferAttachments[2] = normalImageView;
	mFrameBufferAttachments[3] = roughnessImageView;
	mFrameBufferAttachments[4] = depthImageView;

	createFramebufferAttachmentSampler();
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
	VK_CHECK( vkEnumeratePhysicalDevices(mInstance, &gpuCount, gpuList.data()) );

	// Only taking the first GPU found
	mActiveGpu = gpuList[0];

	vkGetPhysicalDeviceMemoryProperties(mActiveGpu, &mGpuMemoryProperties);
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
	const float qPriority[] = { 1.0f };

	VkDeviceQueueCreateInfo queueInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	queueInfo.queueFamilyIndex = mActiveQueue;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = qPriority;

	VkDeviceCreateInfo deviceInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	deviceInfo.ppEnabledExtensionNames = requiredExtensions.data();

	VK_CHECK( vkCreateDevice(mActiveGpu, &deviceInfo, nullptr, &mActiveDevice) );
	volkLoadDevice(mActiveDevice);

	// Getting first queue only
	vkGetDeviceQueue(mActiveDevice, mActiveQueue, 0, &mActiveDeviceQueue);

	VkPhysicalDeviceProperties deviceProps;
	vkGetPhysicalDeviceProperties(mActiveGpu, &deviceProps);

	MiniEngine::Logger::print("Device Name: {}, Driver Version: {}", deviceProps.deviceName, deviceProps.driverVersion);
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
		[](VkSurfaceFormatKHR currFormat) { return currFormat.format == PREFERRED_FORMAT; });

	if (iter == supportedFormatList.end())
	{
		iter = supportedFormatList.begin();
		MiniEngine::Logger::wprint("{} not found as a supported format. Defaulting to {}", PREFERRED_FORMAT, (*iter).format);
	}

	auto format = *iter;

	mCurrentSwapchainFormat = format.format;

	// Choose desired depth as well
	constexpr VkFormat depthList[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};

	mCurrentSwapchainDepthFormat = depthList[0];

	for(auto dFormat : depthList)
	{ 
		VkFormatProperties dProps;
		vkGetPhysicalDeviceFormatProperties(mActiveGpu, dFormat, &dProps);
		
		if (dProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			mCurrentSwapchainDepthFormat = dFormat;
			break;
		}
	}

	VkExtent2D swapchainSize;

	if (surfaceProperties.currentExtent.width == 0xFFFFFFFF)
	{
		swapchainSize.width = mParams.screenWidth;
		swapchainSize.height = mParams.screenHeight;
	}
	else
	{
		swapchainSize = surfaceProperties.currentExtent;
		mParams.screenHeight = swapchainSize.height;
		mParams.screenWidth = swapchainSize.width;
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

	mSwapchainPerImageData = std::vector<PerFrameData>(imageCount, PerFrameData{});

	// Initialize a command pool per swapchain image
	for (uint32_t i = 0; i < imageCount; ++i)
	{
		VkFence fence;
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;

		VkFenceCreateInfo imageFenceInfo { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		imageFenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VK_CHECK( vkCreateFence(mActiveDevice, &imageFenceInfo, nullptr, &fence) );

		VkCommandPoolCreateInfo cmdPoolInfo { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		cmdPoolInfo.queueFamilyIndex = mActiveQueue;
		VK_CHECK( vkCreateCommandPool(mActiveDevice, &cmdPoolInfo, nullptr, &commandPool) );

		VkCommandBufferAllocateInfo cmdBuffInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		cmdBuffInfo.commandPool = commandPool;
		cmdBuffInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBuffInfo.commandBufferCount = 1;
		VK_CHECK( vkAllocateCommandBuffers(mActiveDevice, &cmdBuffInfo, &commandBuffer) );

		mSwapchainPerImageData[i].imageFence = fence;
		mSwapchainPerImageData[i].imageCommandPool = commandPool;
		mSwapchainPerImageData[i].imageCommandBuffer = commandBuffer;
	}

	// Create and image we can render to
	for (uint32_t i = 0; i < imageCount; ++i)
	{
		VkImageView imageView;

		VkImageViewCreateInfo viewInfo { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
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

		VK_CHECK( vkCreateImageView(mActiveDevice, &viewInfo, nullptr, &imageView) );
		mSwapchainPerImageData[i].imageView = imageView;
	}
}

void MiniEngine::Backend::VulkanDriver::createRenderPass()
{
	std::array<VkAttachmentDescription, 5> attachmentDescs = {};

	for (uint32_t i = 0; i < attachmentDescs.size(); ++i)
	{
		attachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (i == attachmentDescs.size() - 1) // Depth target
		{
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachmentDescs[i].format = mCurrentSwapchainDepthFormat;
		}
		else
		{
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentDescs[i].format = mCurrentSwapchainFormat;
		}
	}

	std::array<VkAttachmentReference, attachmentDescs.size() - 1> colorReferences = {};
	VkAttachmentReference depthReference = {};

	colorReferences[0] = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	colorReferences[1] = { 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	colorReferences[2] = { 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	colorReferences[3] = { 3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

	depthReference.attachment = 4;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDesc = {};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.colorAttachmentCount = colorReferences.size();
	subpassDesc.pColorAttachments = colorReferences.data();
	subpassDesc.pDepthStencilAttachment = &depthReference;

	std::array<VkSubpassDependency, 1> subpassDeps;
	subpassDeps[0].srcSubpass = 0;
	subpassDeps[0].dstSubpass = VK_SUBPASS_EXTERNAL;
	subpassDeps[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	subpassDeps[0].dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	subpassDeps[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	subpassDeps[0].dstAccessMask = 0;
	subpassDeps[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo rpInfo { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	rpInfo.attachmentCount = attachmentDescs.size();
	rpInfo.pAttachments = attachmentDescs.data();
	rpInfo.subpassCount = 1;
	rpInfo.pSubpasses = &subpassDesc;
	rpInfo.dependencyCount = 1;
	rpInfo.pDependencies = subpassDeps.data();

	VK_CHECK( vkCreateRenderPass(mActiveDevice, &rpInfo, nullptr, &mDefaultRenderpass) );
}

void MiniEngine::Backend::VulkanDriver::createPipeline()
{
	// Descriptor Sets ===============================

	VkDescriptorSetLayout defaultLayout;
	VkDescriptorSetLayoutCreateInfo descriptorSetInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	descriptorSetInfo.bindingCount = 0;
	descriptorSetInfo.pBindings = nullptr;

	VK_CHECK(vkCreateDescriptorSetLayout(mActiveDevice, &descriptorSetInfo, nullptr, &defaultLayout));

	// ===============================================

	VkPipelineLayoutCreateInfo layoutInfo { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &defaultLayout;
	VK_CHECK( vkCreatePipelineLayout(mActiveDevice, &layoutInfo, nullptr, &mDefaultPipelineLayout) );

	VkPipelineVertexInputStateCreateInfo vertexInputInfo { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineRasterizationStateCreateInfo rasterInfo { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterInfo.lineWidth = 1.0f;

	VkPipelineColorBlendAttachmentState colorBlendState{};
	colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendAttachmentState blendList[] = { colorBlendState, colorBlendState, colorBlendState, colorBlendState };

	VkPipelineColorBlendStateCreateInfo colorBlendInfo { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	colorBlendInfo.attachmentCount = 4;
	colorBlendInfo.pAttachments = blendList;

	VkPipelineViewportStateCreateInfo viewportInfo { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewportInfo.viewportCount = 1;
	viewportInfo.scissorCount = 1;

	VkPipelineDepthStencilStateCreateInfo depthStencilInfo { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	depthStencilInfo.depthTestEnable = true;
	depthStencilInfo.depthWriteEnable = true;

	VkPipelineMultisampleStateCreateInfo multiSampleInfo { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multiSampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Change to static
	std::array<VkDynamicState, 2> dynamics { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	
	VkPipelineDynamicStateCreateInfo dynamicInfo { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamicInfo.pDynamicStates = dynamics.data();
	dynamicInfo.dynamicStateCount = static_cast<uint32_t>(dynamics.size());

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module =
		MiniTools::GlslCompiler::loadShader(
			RESOLVE_PATH("/shaders/temp.vert"),
			VK_SHADER_STAGE_VERTEX_BIT,
			mActiveDevice);

	shaderStages[0].pName = "main";

	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = 
		MiniTools::GlslCompiler::loadShader(
			RESOLVE_PATH("/shaders/temp.frag"),
			VK_SHADER_STAGE_FRAGMENT_BIT,
			mActiveDevice);

	shaderStages[1].pName = "main";

	VkGraphicsPipelineCreateInfo pipelineInfo { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pRasterizationState = &rasterInfo;
	pipelineInfo.pColorBlendState = &colorBlendInfo;
	pipelineInfo.pMultisampleState = &multiSampleInfo;
	pipelineInfo.pViewportState = &viewportInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.pDynamicState = &dynamicInfo;

	pipelineInfo.renderPass = mDefaultRenderpass;
	pipelineInfo.layout = mDefaultPipelineLayout;

	VK_CHECK( vkCreateGraphicsPipelines(mActiveDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mDefaultPipeline) );

	vkDestroyShaderModule(mActiveDevice, shaderStages[0].module, nullptr);
	vkDestroyShaderModule(mActiveDevice, shaderStages[1].module, nullptr);
}

void MiniEngine::Backend::VulkanDriver::createFramebuffer()
{
	VkFramebufferCreateInfo fbInfo { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	fbInfo.renderPass = mDefaultRenderpass;
	fbInfo.attachmentCount = mFrameBufferAttachments.size();
	fbInfo.pAttachments = mFrameBufferAttachments.data();
	fbInfo.width = mParams.screenWidth;
	fbInfo.height = mParams.screenHeight;
	fbInfo.layers = 1;

	VK_CHECK( vkCreateFramebuffer(mActiveDevice, &fbInfo, nullptr, &mFramebuffer) );
}

void MiniEngine::Backend::VulkanDriver::createFramebufferAttachmentSampler()
{
	VkSampler colorSampler;

	VkSamplerCreateInfo samplerInfo { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerInfo.magFilter = VK_FILTER_NEAREST;
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.maxAnisotropy = 1.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

	VK_CHECK( vkCreateSampler(mActiveDevice, &samplerInfo, nullptr, &colorSampler) );
}

VkImageView MiniEngine::Backend::VulkanDriver::createImageAttachment(VkFormat imageFormat, VkImageUsageFlags imageBits, VkImageAspectFlags imageViewAspectFlags)
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

	VK_CHECK(vkCreateImage(mActiveDevice, &attachmentImageInfo, nullptr, &attachmentImage));
	vkGetImageMemoryRequirements(mActiveDevice, attachmentImage, &attachmentMemReqs);

	// Get memory info

	uint32_t index = 0;
	auto typeBits = attachmentMemReqs.memoryTypeBits;

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

	attachmentAllocateInfo.allocationSize = attachmentMemReqs.size;
	attachmentAllocateInfo.memoryTypeIndex = index;
	VK_CHECK(vkAllocateMemory(mActiveDevice, &attachmentAllocateInfo, nullptr, &attachmentMemory));
	VK_CHECK(vkBindImageMemory(mActiveDevice, attachmentImage, attachmentMemory, 0));

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
	VK_CHECK(vkCreateImageView(mActiveDevice, &colorImageViewInfo, nullptr, &attachmentImageView));

	return attachmentImageView;
}

void MiniEngine::Backend::VulkanDriver::loadShaderModule()
{

}

void MiniEngine::Backend::VulkanDriver::acquireNextImage(uint32_t* image)
{
	//TODO need multiple semaphores
	static VkSemaphore acquireSemaphore = VK_NULL_HANDLE;
	VkSemaphoreCreateInfo semaphoreInfo { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

	if (acquireSemaphore == VK_NULL_HANDLE)
		VK_CHECK( vkCreateSemaphore(mActiveDevice, &semaphoreInfo, nullptr, &acquireSemaphore) );

	VkResult res = vkAcquireNextImageKHR(mActiveDevice, mActiveSwapchain, UINT64_MAX, acquireSemaphore, VK_NULL_HANDLE, image);

	mSwapchainPerImageData[*image].acquireSemaphore = acquireSemaphore;

	if (res != VK_SUCCESS)
	{
		MiniEngine::Logger::eprint("Image not acquired");
		return;
	}

	if (mSwapchainPerImageData[*image].imageFence != VK_NULL_HANDLE)
	{
		vkWaitForFences(mActiveDevice, 1, &(mSwapchainPerImageData[*image].imageFence), true, UINT64_MAX);
		vkResetFences(mActiveDevice, 1, &(mSwapchainPerImageData[*image].imageFence));
	}

	if (mSwapchainPerImageData[*image].imageCommandPool != VK_NULL_HANDLE)
	{
		vkResetCommandPool(mActiveDevice, mSwapchainPerImageData[*image].imageCommandPool, 0);
	}
}

// ==== Interface ====

void MiniEngine::Backend::VulkanDriver::draw(MiniEngine::Scene* scene)
{
	uint32_t imgIndex;
	acquireNextImage(&imgIndex);

	VkCommandBuffer cmd = mSwapchainPerImageData[imgIndex].imageCommandBuffer;

	VkCommandBufferBeginInfo beginInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmd, &beginInfo);

	VkClearValue clearValue{};
	auto color = mParams.clearColor;
	clearValue.color = { { color.r(), color.g(), color.b(), color.a() } };
	clearValue.depthStencil.depth = 0;
	clearValue.depthStencil.stencil = 0;

	VkClearValue clearValues[] = { clearValue, clearValue, clearValue, clearValue, clearValue };

	VkRenderPassBeginInfo rpBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	rpBeginInfo.renderPass = mDefaultRenderpass;
	rpBeginInfo.framebuffer = mFramebuffer;
	rpBeginInfo.renderArea.extent.width = mParams.screenWidth;
	rpBeginInfo.renderArea.extent.height = mParams.screenHeight;
	rpBeginInfo.clearValueCount = 5;
	rpBeginInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass(cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mDefaultPipeline); //need to bind once?

	VkViewport vp{};
	vp.width = mParams.screenWidth;
	vp.height = mParams.screenHeight;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;

	vkCmdSetViewport(cmd, 0, 1, &vp);
	
	VkRect2D scissor{};
	scissor.extent.width = mParams.screenWidth;
	scissor.extent.height = mParams.screenHeight;

	vkCmdSetScissor(cmd, 0, 1, &scissor);
	vkCmdDraw(cmd, 3, 5, 0, 0);
	vkCmdEndRenderPass(cmd);

	VK_CHECK( vkEndCommandBuffer(cmd) );

	if (mSwapchainPerImageData[imgIndex].releaseSemaphore == VK_NULL_HANDLE)
	{
		VkSemaphoreCreateInfo semInfo { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VK_CHECK( vkCreateSemaphore(mActiveDevice, &semInfo, nullptr, &mSwapchainPerImageData[imgIndex].releaseSemaphore) );
	}

	VkPipelineStageFlags waitStageFlags { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &mSwapchainPerImageData[imgIndex].acquireSemaphore;
	submitInfo.pWaitDstStageMask = &waitStageFlags;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &mSwapchainPerImageData[imgIndex].releaseSemaphore;

	VK_CHECK( vkQueueSubmit(mActiveDeviceQueue, 1, &submitInfo, mSwapchainPerImageData[imgIndex].imageFence) );

	// Present
	VkPresentInfoKHR presentInfo { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &mActiveSwapchain;
	presentInfo.pImageIndices = &imgIndex;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &mSwapchainPerImageData[imgIndex].releaseSemaphore;

	vkQueuePresentKHR(mActiveDeviceQueue, &presentInfo);
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
