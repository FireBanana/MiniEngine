#include "VulkanDriver.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type,
	uint64_t object, size_t location, int32_t messageCode, const char* layerPrefix, const char* message, void* userData)
{
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		MiniEngine::Logger::eprint("Validation Layer: Error: {}: {}", layerPrefix, message);
	else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		MiniEngine::Logger::wprint("Validation Layer: Warning: {}: {}", layerPrefix, message);
	else
		MiniEngine::Logger::wprint("Validation Layer: Info: {}: {}", layerPrefix, message);

	return VK_FALSE;
;}

void MiniEngine::Backend::VulkanDriver::initialize()
{
	VK_CHECK(volkInitialize());

	enumerateInstanceExtensionProperties();
	enumerateInstanceLayerProperties();
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
	VkDebugReportCallbackEXT debug_callback = VK_NULL_HANDLE;

	VkDebugReportCallbackCreateInfoEXT debugInfo{ VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
	debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT   | 
					  VK_DEBUG_REPORT_WARNING_BIT_EXT | 
					  VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

	debugInfo.pfnCallback = debugCallback;
	
	//info.pNext = &debugInfo;
#endif

	VK_CHECK( vkCreateInstance(&info, nullptr, &mInstance) );

	volkLoadInstance(mInstance);

#ifdef GRAPHICS_DEBUG
	//VK_CHECK(vkCreateDebugReportCallbackEXT(mInstance, &debugInfo, nullptr, &debug_callback));
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

	VK_CHECK( vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr) );
	std::vector<VkLayerProperties> layerPropertyList(instanceLayerCount);
	VK_CHECK( vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerPropertyList.data()) );

	for (auto i : layerPropertyList)
	{
		MiniEngine::Logger::print(i.layerName);
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
