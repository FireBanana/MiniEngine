#include "ShaderRegistry.h"
#include "../backend/OpenGLDriver.h"
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

void ShaderRegistry::loadPbrShader()
{
	auto vertexShader = mDriver->loadShader(LIGHTING_VERTEX_PATH, OpenGLDriver::ShaderType::VERTEX);
	auto fragmentShader = mDriver->loadShader(LIGHTING_FRAGMENT_PATH, OpenGLDriver::ShaderType::FRAGMENT);
	auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

	createShader(shader, mDriver);
}

void ShaderRegistry::setActiveUniformBuffer(const char* blockName, size_t dataSize, void* data)
{
	mDriver->createUniformBlock(mActiveShader, dataSize, data);
	mDriver->registerUniformBlock(blockName, mActiveShader);
}

void ShaderRegistry::enable(Shader* shader)
{
	mDriver->useShaderProgram(shader->getShaderProgram());
	mActiveShader = shader;
}

void ShaderRegistry::createShader(unsigned int program, const OpenGLDriver* driver)
{
	mShaderTable.push_back(Shader{ program, driver });
}

