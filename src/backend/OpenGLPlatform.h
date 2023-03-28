#pragma once

#include <memory>
#include <GLFW/glfw3.h>
#include "OpenGLDriver.h"
#include "../core/Engine.h"
#include"../core/utils/Color.h"
#include"../core/ShaderRegistry.h"

class OpenGLPlatform
{
public:

	OpenGLPlatform(const EngineInitParams& params);

	void makeCurrent();
	void execute();

private:

	std::unique_ptr<OpenGLDriver> mDriver;
	GLFWwindow*					  mWindow;
	ShaderRegistry				  mShaderRegistry;

	void createWindow(uint16_t width, uint16_t height);
	void createDriver(const EngineInitParams& params);
};