#include "ShaderRegistry.h"
#include "../backend/OpenGLDriver.h"

ShaderRegistry::ShaderRegistry(const OpenGLDriver* driver)
	: mDriver(driver)
{
}

void ShaderRegistry::loadDeferredShader()
{
	auto vertexShader = mDriver->loadShader(DEFERRED_VERTEX_PATH, OpenGLDriver::ShaderType::VERTEX);
	auto fragmentShader = mDriver->loadShader(DEFERRED_FRAGMENT_PATH, OpenGLDriver::ShaderType::FRAGMENT);
	auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

	mDefaultShader = { shader, mDriver };
}

