#pragma once

#include <memory>
#include <GLFW/glfw3.h>

#include "OpenGLDriver.h"
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

			OpenGLPlatform(const EngineInitParams& params, Engine* engine);
			OpenGLPlatform(OpenGLPlatform&) = delete;
			OpenGLPlatform(OpenGLPlatform&&) = delete;

			void makeCurrent();
			void execute(Scene* scene);

			inline OpenGLDriver* getDriver() const { return mDriver.get(); }

		private:

			std::unique_ptr<OpenGLDriver> mDriver;
			GLFWwindow* mWindow;
			Engine* mEngine;

			void createWindow(uint16_t width, uint16_t height);
			void createDriver(const EngineInitParams& params);
		};
	}
}