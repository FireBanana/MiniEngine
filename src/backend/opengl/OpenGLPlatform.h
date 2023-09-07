#pragma once

#include <memory>
#include <GLFW/glfw3.h>

#include "IPlatform.h"
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
		class OpenGLPlatform : public IPlatform
		{
		public:

			void initialize(MiniEngine::Types::EngineInitParams& params, Engine* engine) override;

			void makeCurrent() override;
			void execute(Scene* scene) override;

			inline OpenGLDriver* getDriver() const override { return mDriver.get(); }
			inline OpenGLImgui* getUiInterface() const override { return mImgui.get(); }

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