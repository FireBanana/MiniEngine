#include "VulkanDriver.h"
#include "FileHelper.h"
#include "GlslCompiler.h"
#include "Scene.h"
#include "ShaderData.h"
#include "VulkanImage.h"
#include "VulkanPipeline.h"
#include "VulkanRenderDoc.h"
#include "VulkanSwapchain.h"
#include <glm/gtc/matrix_transform.hpp>

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	return false;
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

void MiniEngine::Backend::VulkanDriver::generateDevice()
{
	registerPhysicalDevice();
	registerPhysicalDeviceQueueFamily();
	enumerateDeviceExtensionProperties();
	createDevice({ "VK_KHR_swapchain",
				  "VK_KHR_dynamic_rendering",
				  "VK_KHR_depth_stencil_resolve",
				  "VK_KHR_create_renderpass2",
				  "VK_KHR_multiview",
				  "VK_KHR_maintenance2" });

	initializeMemoryAllocator();
}

void MiniEngine::Backend::VulkanDriver::generateSwapchain()
{
	createSwapchain();
	createDisplaySemaphores();
}

void MiniEngine::Backend::VulkanDriver::generatePipelines()
{
	createDescriptorPools();
	createGBufferPipeline();
	createLightingPipeline();
}

void MiniEngine::Backend::VulkanDriver::generateGbuffer()
{
	auto color = VulkanImage::Builder(this)
		.setWidth(mParams.screenWidth)
		.setHeight(mParams.screenHeight)
		.setChannels(4)
		.setUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT
			| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
		.setFormat(mActiveSwapchain.getFormat())
		.setData(nullptr)
		.setDebugName("Color Target")
		.build();

	auto position = VulkanImage::Builder(this)
		.setWidth(mParams.screenWidth)
		.setHeight(mParams.screenHeight)
		.setChannels(4)
		.setUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT
			| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
		.setFormat(mActiveSwapchain.getFormat())
		.setData(nullptr)
		.setDebugName("Position Target")
		.build();

	auto normal = VulkanImage::Builder(this)
		.setWidth(mParams.screenWidth)
		.setHeight(mParams.screenHeight)
		.setChannels(4)
		.setUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT
			| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
		.setFormat(mActiveSwapchain.getFormat())
		.setData(nullptr)
		.setDebugName("Normal Target")
		.build();

	auto roughness = VulkanImage::Builder(this)
		.setWidth(mParams.screenWidth)
		.setHeight(mParams.screenHeight)
		.setChannels(4)
		.setUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT
			| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
		.setFormat(mActiveSwapchain.getFormat())
		.setData(nullptr)
		.setDebugName("Roughness Target")
		.build();

	auto depth = VulkanImage::Builder(this)
		.setWidth(mParams.screenWidth)
		.setHeight(mParams.screenHeight)
		.setChannels(4)
		.setUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT
			| VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		.setAspectFlags(VK_IMAGE_ASPECT_DEPTH_BIT)
		.setFormat(mActiveSwapchain.getDepthFormat())
		.setData(nullptr)
		.setDebugName("Depth Target")
		.build();

	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::COLOR)] = color;
	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::POSITION)] = position;
	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::NORMAL)] = normal;
	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::ROUGHNESS)] = roughness;
	mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::DEPTH)] = depth;
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
	auto swapchain = VulkanSwapchain::Builder(this)
		.setHeight(mParams.screenHeight)
		.setWidth(mParams.screenWidth)
		.setPreferredDepthFormat(VK_FORMAT_D32_SFLOAT)
		.setPreferredFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
		.setPresentMode(VK_PRESENT_MODE_FIFO_KHR)
		.setSurface(mSurface)
		.build();

	mActiveSwapchain = swapchain;
}

void MiniEngine::Backend::VulkanDriver::createDescriptorPools()
{
	VkDescriptorPoolSize uniformScenePoolSize{};
	VkDescriptorPoolSize imagePoolSize{};

	// view, projection, and camera position
	uniformScenePoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformScenePoolSize.descriptorCount = 2;

	imagePoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	imagePoolSize.descriptorCount = 5;

	VkDescriptorPoolCreateInfo uniformDescriptorPoolInfo{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	VkDescriptorPoolCreateInfo imageDescriptorPoolInfo{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };

	uniformDescriptorPoolInfo.poolSizeCount = 1;
	uniformDescriptorPoolInfo.maxSets = 2;
	uniformDescriptorPoolInfo.pPoolSizes = &uniformScenePoolSize;

	imageDescriptorPoolInfo.poolSizeCount = 1;
	imageDescriptorPoolInfo.maxSets = 2;
	imageDescriptorPoolInfo.pPoolSizes = &imagePoolSize;

	vkCreateDescriptorPool(mActiveDevice, &uniformDescriptorPoolInfo, nullptr, &mDescriptorPools[0]);
	vkCreateDescriptorPool(mActiveDevice, &imageDescriptorPoolInfo, nullptr, &mDescriptorPools[1]);
}

void MiniEngine::Backend::VulkanDriver::createGBufferPipeline()
{
	SceneBlock sceneBlock{};
	sceneBlock.cameraPosition = glm::vec3(0, 0, 2);
	sceneBlock.projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	sceneBlock.view = glm::lookAt(sceneBlock.cameraPosition,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f));

	auto sceneBlockBuffer = createBuffer(sizeof(sceneBlock),
		&sceneBlock,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT, "GBufferUniformBuffer");

	auto sceneDescriptorSet = VulkanDescriptorSet::Builder(this)
		.setBinding(0)
		.setCount(1)
		.setShaderStages(VK_SHADER_STAGE_VERTEX_BIT
			| VK_SHADER_STAGE_FRAGMENT_BIT)
		.setType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		.setPool(mDescriptorPools[0])
		.setDebugName("GBufferUniformDescriptor")
		.build();

	auto imageBufferDescriptorSet = VulkanDescriptorSet::Builder(this)
		.setBinding(1)
		.setCount(3)
		.setShaderStages(VK_SHADER_STAGE_FRAGMENT_BIT)
		.setType(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
		.setPool(mDescriptorPools[1])
		.setDebugName("GBufferImageDescriptor")
		.build();

	sceneDescriptorSet.loadData(std::move(sceneBlockBuffer));
	sceneDescriptorSet.update();

	mGbufferPipeline = VulkanPipeline::Builder(this)
		.setAttachmentCount(4)
		.addShaderState(DIR "/shaders/deferred.vert",
			DIR "/shaders/deferred.frag")
		.addVertexAttributeState(0, { 2, 3 }) //point, color
		.addDescriptorSet(std::move(sceneDescriptorSet))
		.addDescriptorSet(std::move(imageBufferDescriptorSet))
		.setDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR })
		.setRasterState(true, true)
		.setDepthState(true, true)
		.build();
}

void MiniEngine::Backend::VulkanDriver::createLightingPipeline()
{
	auto descriptorSet = VulkanDescriptorSet::Builder(this)
		.setBinding(0)
		.setCount(1)
		.setShaderStages(VK_SHADER_STAGE_FRAGMENT_BIT)
		.setType(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
		.setPool(mDescriptorPools[1])
		.setDebugName("LightingImageDescriptor")
		.build();

	descriptorSet.loadData(&mImageAttachments[0]);
	descriptorSet.update();

	mLightingPipeline = VulkanPipeline::Builder(this)
		.setAttachmentCount(5)
		.addShaderState(DIR "/shaders/lighting_vk.vert",
			DIR "/shaders/lighting_vk.frag")
		.setDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR })
		.setRasterState(true, true)
		.setDepthState(false, false)
		.addDescriptorSet(std::move(descriptorSet))
		.build();
}

void MiniEngine::Backend::VulkanDriver::createDisplaySemaphores()
{
	mDisplaySemaphoreArray = Utils::DynamicArray<DisplaySemaphore>(
		mActiveSwapchain.getSwapchainSize());

	auto arrayPtr = mDisplaySemaphoreArray.get();

	VkSemaphoreCreateInfo info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	fInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < mActiveSwapchain.getSwapchainSize(); ++i) {
		vkCreateSemaphore(mActiveDevice, &info, nullptr, &(arrayPtr[i].acquisitionSemaphore));
		vkCreateSemaphore(mActiveDevice, &info, nullptr, &(arrayPtr[i].presentationSemaphore));
		vkCreateFence(mActiveDevice, &fInfo, nullptr, &(arrayPtr[i].fence));
	}
}

void MiniEngine::Backend::VulkanDriver::initializeMemoryAllocator()
{
	VmaVulkanFunctions vulkFuncs{};
	vulkFuncs.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
	vulkFuncs.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
	vulkFuncs.vkAllocateMemory = vkAllocateMemory;
	vulkFuncs.vkFreeMemory = vkFreeMemory;
	vulkFuncs.vkMapMemory = vkMapMemory;
	vulkFuncs.vkUnmapMemory = vkUnmapMemory;
	vulkFuncs.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
	vulkFuncs.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
	vulkFuncs.vkBindBufferMemory = vkBindBufferMemory;
	vulkFuncs.vkBindImageMemory = vkBindImageMemory;
	vulkFuncs.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
	vulkFuncs.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
		vulkFuncs.vkCreateBuffer = vkCreateBuffer;
	vulkFuncs.vkDestroyBuffer = vkDestroyBuffer;
	vulkFuncs.vkCreateImage = vkCreateImage;
	vulkFuncs.vkDestroyImage = vkDestroyImage;
	vulkFuncs.vkCmdCopyBuffer = vkCmdCopyBuffer;
	vulkFuncs.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
	vulkFuncs.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;

	VmaAllocatorCreateInfo vmaInfo{};
	vmaInfo.device = mActiveDevice;
	vmaInfo.instance = mInstance;
	vmaInfo.physicalDevice = mActiveGpu;
	vmaInfo.pVulkanFunctions = &vulkFuncs;

	vmaCreateAllocator(&vmaInfo, &mMemoryAllocator);
}

void MiniEngine::Backend::VulkanDriver::recordCommandBuffers(MiniEngine::Scene* scene)
{
	// TODO: refactor into renderpass
	VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	auto perFrameData = mActiveSwapchain.getPerFrameData();

	for (auto i = 0; i < mActiveSwapchain.getSwapchainSize(); ++i) {
		auto& cmd = perFrameData[i].imageCommandBuffer;

		vkBeginCommandBuffer(cmd, &beginInfo);

		auto swapchainImage = perFrameData[i].rawImage;
		auto colorImage = mImageAttachments[static_cast<int>(ImageAttachmentType::COLOR)]
			.getRawImage();
		auto positionImage = mImageAttachments[static_cast<int>(ImageAttachmentType::POSITION)]
			.getRawImage();
		auto normalImage = mImageAttachments[static_cast<int>(ImageAttachmentType::NORMAL)].getRawImage();
		auto roughnessImage = mImageAttachments[static_cast<int>(ImageAttachmentType::ROUGHNESS)].getRawImage();

		auto clearColor = VkClearColorValue{ mParams.clearColor.r(),
											mParams.clearColor.g(),
											mParams.clearColor.b(),
											mParams.clearColor.a() };

		VkRenderingAttachmentInfo gBufferColorAttachment{
			VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		gBufferColorAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::COLOR)].getImageView();
		gBufferColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		gBufferColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferColorAttachment.clearValue.color = clearColor;

		VkRenderingAttachmentInfo gBufferPositionAttachment{
			VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		gBufferPositionAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::POSITION)].getImageView();
		gBufferPositionAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		gBufferPositionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferPositionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferPositionAttachment.clearValue.color = clearColor;

		VkRenderingAttachmentInfo gBufferNormalAttachment{
			VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		gBufferNormalAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::NORMAL)].getImageView();
		gBufferNormalAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		gBufferNormalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferNormalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferNormalAttachment.clearValue.color = clearColor;

		VkRenderingAttachmentInfo gBufferRoughnessAttachment{
			VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		gBufferRoughnessAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::ROUGHNESS)].getImageView();
		gBufferRoughnessAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		gBufferRoughnessAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferRoughnessAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferRoughnessAttachment.clearValue.color = clearColor;

		VkRenderingAttachmentInfo depthAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		depthAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::DEPTH)].getImageView();
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.clearValue.depthStencil = { 1, 0 };

		VkRenderingAttachmentInfo gBufferAttachmentArray[] = { gBufferColorAttachment,
													   gBufferPositionAttachment,
													   gBufferNormalAttachment,
													   gBufferRoughnessAttachment };

		mGbufferPipeline.bind(cmd);

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

		VkRenderingInfoKHR gBufferRenderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
		gBufferRenderingInfo.layerCount = 1;
		gBufferRenderingInfo.colorAttachmentCount = 4;
		gBufferRenderingInfo.pColorAttachments = gBufferAttachmentArray;
		gBufferRenderingInfo.pDepthAttachment = &depthAttachment;
		gBufferRenderingInfo.renderArea = { 0, 0, mParams.screenWidth, mParams.screenHeight };

		auto firstCamera = scene->getCameraComponentDatabase()[0]; //TODO: fix

		SceneBlock sceneBlock{};
		sceneBlock.cameraPosition = glm::vec3(firstCamera.position.x,
			firstCamera.position.y,
			firstCamera.position.z);
		sceneBlock.projection = glm::perspective(glm::radians(firstCamera.fov),
			firstCamera.aspectRatio,
			firstCamera.nearPlane,
			firstCamera.farPlane);
		sceneBlock.view = glm::lookAt(glm::vec3(firstCamera.position.x,
			firstCamera.position.y,
			firstCamera.position.z),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f));

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

		/*createPipelineBarrier(colorImage,
			cmd,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);*/

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

		// GBuffer pass
		vkCmdUpdateBuffer(cmd,
			mGbufferPipeline.mDescriptors[0].mBuffers[0].getRawBuffer(),
			0,
			sizeof(SceneBlock),
			&sceneBlock);

		vkCmdBeginRendering(cmd, &gBufferRenderingInfo);
		auto renderables = scene->getRenderableComponentDatabase();
		for (size_t i = 0; i < renderables.size(); ++i) {
			VkDeviceSize offset = { 0 };
			auto buffer = renderables[i].vbuffer.getRawBuffer();
			vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &offset);
			vkCmdDraw(cmd, 3, 1, 0, 0);
		}
		vkCmdEndRendering(cmd);

		// ================ LIGHTING ==================================================================================

		VkRenderingAttachmentInfo lightingSwapChainAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingSwapChainAttachment.imageView = perFrameData[i].imageView;
		lightingSwapChainAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingSwapChainAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		lightingSwapChainAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		lightingSwapChainAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo lightingColorAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingColorAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::COLOR)].getImageView();
		lightingColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		lightingColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		lightingColorAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo lightingPositionAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingPositionAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::POSITION)].getImageView();
		lightingPositionAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingPositionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		lightingPositionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		lightingPositionAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo lightingNormalAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingNormalAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::NORMAL)].getImageView();
		lightingNormalAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingNormalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		lightingNormalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		lightingNormalAttachment.clearValue.color = { 1, 0, 0, 1 };

		VkRenderingAttachmentInfo lightingRoughnessAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		lightingRoughnessAttachment.imageView
			= mImageAttachments[static_cast<unsigned int>(ImageAttachmentType::ROUGHNESS)].getImageView();
		lightingRoughnessAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightingRoughnessAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		lightingRoughnessAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		lightingRoughnessAttachment.clearValue.color = { 1, 0, 0, 1 };

		auto lightingAttachmentArray = { lightingColorAttachment, lightingPositionAttachment, lightingNormalAttachment,
			lightingRoughnessAttachment, lightingSwapChainAttachment };

		mLightingPipeline.bind(cmd);

		VkRenderingInfoKHR lightingRenderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
		lightingRenderingInfo.layerCount = 1;
		lightingRenderingInfo.colorAttachmentCount = 5;
		lightingRenderingInfo.pColorAttachments = lightingAttachmentArray.begin();
		lightingRenderingInfo.renderArea = { 0, 0, mParams.screenWidth, mParams.screenHeight };

		createPipelineBarrier(colorImage,
			cmd,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

		// Create new attachment array here with different load store
		// Lighting pass
		vkCmdBeginRendering(cmd, &lightingRenderingInfo);
		vkCmdDraw(cmd, 6, 1, 0, 0);
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
		mActiveSwapchain.getSwapchain(),
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
	size_t memSize, void* data, VkBufferUsageFlags usageFlags, std::string&& debugName)
{
	VulkanBuffer buffer{ this, mGpuMemoryProperties };
	buffer.create(mMemoryAllocator, memSize, data, usageFlags, std::move(debugName));
	return buffer;
}

void MiniEngine::Backend::VulkanDriver::draw(MiniEngine::Scene* scene)
{
	uint32_t imgIndex, displaySemaphoreIndex;
	acquireNextImage(&imgIndex, &displaySemaphoreIndex);
	auto perFrameData = mActiveSwapchain.getPerFrameData();

	auto& cmd = perFrameData[imgIndex].imageCommandBuffer;

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

	auto swapChain = mActiveSwapchain.getSwapchain();

	// Present
	VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain;
	presentInfo.pImageIndices = &imgIndex;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &(
		mDisplaySemaphoreArray.get()[displaySemaphoreIndex].presentationSemaphore);

	vkQueuePresentKHR(mActiveDeviceQueue, &presentInfo);
}

unsigned int MiniEngine::Backend::VulkanDriver::createTexture(
	int width, int height, int channels, void* data, TextureType type)
{
	auto texture = VulkanImage::Builder(this)
		.setWidth(mParams.screenWidth)
		.setHeight(mParams.screenHeight)
		.setChannels(4)
		.setUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT
			| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
		.setFormat(mActiveSwapchain.getFormat()) // todo change
		.setData(data)
		.build();

	// todo remove, always adding to gbuffer textures
	mGbufferPipeline.mDescriptors[1].loadData(&texture);

	return 0;
}

void MiniEngine::Backend::VulkanDriver::setupMesh(MiniEngine::Components::RenderableComponent* component)
{
	auto vertexBuffer = createBuffer(component->buffer.size() * sizeof(float),
		component->buffer.data(),
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	component->vbuffer = std::move(vertexBuffer);
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
