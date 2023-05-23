#pragma once

#include <memory>
#include <GLFW/glfw3.h>
#include "OpenGLDriver.h"
#include"../core/utils/Color.h"
#include"../core/ShaderRegistry.h"

class Scene;
class Engine;

class OpenGLPlatform
{
public:

	OpenGLPlatform(const EngineInitParams& params, Engine* engine);

	void makeCurrent();
	void execute(Scene* scene);
	
	inline OpenGLDriver* const getDriver() const { return mDriver.get(); }

private:

	std::unique_ptr<OpenGLDriver> mDriver;
	GLFWwindow*					  mWindow;
	Engine*						  mEngine;

	void createWindow(uint16_t width, uint16_t height);
	void createDriver(const EngineInitParams& params);
};