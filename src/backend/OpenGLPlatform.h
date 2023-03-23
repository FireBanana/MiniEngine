#pragma once

#include <memory>
#include <GLFW/glfw3.h>
#include "OpenGLDriver.h"
#include "../core/Engine.h"
#include"../core/utils/Color.h"

class OpenGLPlatform
{
public:

	OpenGLPlatform(const EngineInitParams& params);

	void makeCurrent();
	void execute();

private:

	GLFWwindow* mWindow;
	std::unique_ptr<OpenGLDriver> mDriver;

	void createWindow(uint16_t width, uint16_t height);
	OpenGLDriver* createDriver(const EngineInitParams& params);
};