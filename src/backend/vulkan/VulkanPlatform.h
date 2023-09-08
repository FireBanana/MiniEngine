#pragma once

#include <memory>

#include "VulkanHelper.h"
#include <GLFW/glfw3.h>

#include "VulkanDriver.h"
#include "IPlatform.h"
#include "EngineTypes.h"

namespace MiniEngine
{

class Scene;
class Engine;

namespace Backend
{
	class VulkanPlatform : public IPlatform
	{
	public:

		void initialize(MiniEngine::Types::EngineInitParams& params, Engine* engine) override;
		void makeCurrent() override;
		void execute(Scene* scene) override;
		IImgui* getUiInterface() const override;
		void createImguiInterface() override;

		VulkanDriver* getDriver() const { return mDriver.get(); }

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