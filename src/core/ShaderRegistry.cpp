#include "ShaderRegistry.h"
#include "../backend/OpenGLDriver.h"
#include <algorithm>

ShaderRegistry::ShaderRegistry(const OpenGLDriver* driver)
	: mDriver(driver), mShaderTable()
{
	
}

void ShaderRegistry::loadDeferredShader()
{
	auto vertexShader = mDriver->loadShader(DEFERRED_VERTEX_PATH, OpenGLDriver::ShaderType::VERTEX);
	auto fragmentShader = mDriver->loadShader(DEFERRED_FRAGMENT_PATH, OpenGLDriver::ShaderType::FRAGMENT);
	auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

	createShader(shader, mDriver);
}

void ShaderRegistry::createShader(unsigned int program, const OpenGLDriver* driver)
{
	mShaderTable.push_back(Shader{ program, driver });
}

