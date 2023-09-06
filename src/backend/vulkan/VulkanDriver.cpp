#include "VulkanDriver.h"
#include "VulkanHelper.h"

#include <volk.h>
#include <vulkan/vulkan.h>

void MiniEngine::Backend::VulkanDriver::initialize()
{
	VK_CHECK(volkInitialize());
}

void MiniEngine::Backend::VulkanDriver::createInstance()
{

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
