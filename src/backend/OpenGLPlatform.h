#pragma once

#include <memory>
#include <GLFW/glfw3.h>

#include "OpenGLDriver.h"
#include "OpenGLImgui.h"
#include "Color.h"
#include "ShaderRegistry.h"

namespace MiniEngine
{
	class Scene;
	class Engine;

	namespace Backend
	{
		class OpenGLPlatform
		{
		public:

			OpenGLPlatform(MiniEngine::Types::EngineInitParams& params, Engine* engine);
			OpenGLPlatform(OpenGLPlatform&) = delete;
			OpenGLPlatform(OpenGLPlatform&&) = delete;

			void makeCurrent();
			void execute(Scene* scene);

			inline OpenGLDriver* getDriver() const { return mDriver.get(); }
			inline OpenGLImgui* getUiInterface() const { return mImgui.get(); }

		private:

			std::unique_ptr<OpenGLDriver> mDriver;
			std::unique_ptr<OpenGLImgui>  mImgui;
			GLFWwindow* mWindow;
			Engine* mEngine;

			void createWindow(uint16_t width, uint16_t height);
			void createDriver(MiniEngine::Types::EngineInitParams& params);
			void createImguiInterface();
		};
	}
}