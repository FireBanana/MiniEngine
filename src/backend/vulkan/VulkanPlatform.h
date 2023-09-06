#pragma once

#include <memory>
#include <volk.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "VulkanDriver.h"
#include "EngineTypes.h"

namespace MiniEngine
{

class Scene;
class Engine;

namespace Backend
{
	class VulkanPlatform
	{
	public:

		VulkanPlatform(MiniEngine::Types::EngineInitParams& params, Engine* engine);
		VulkanPlatform(VulkanPlatform&) = delete;
		VulkanPlatform(VulkanPlatform&&) = delete;

		void makeCurrent();
		void execute(Scene* scene);

		VulkanDriver* getDriver() const {  };

	private:

		std::unique_ptr<VulkanDriver> mDriver;
		GLFWwindow* mWindow;
		VkSurfaceKHR mSurface;
		Engine* mEngine;

		void createWindow(uint16_t width, uint16_t height);
		void createDriver(MiniEngine::Types::EngineInitParams& params);
	};
}
}