#include "ShaderRegistry.h"
#include "../backend/OpenGLDriver.h"
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MiniEngine
{
	ShaderRegistry::ShaderRegistry(const Backend::OpenGLDriver* driver)
		: mDriver(driver), mShaderTable()
	{

	}

	void ShaderRegistry::loadDeferredShader()
	{
		auto vertexShader = mDriver->loadShader(DEFERRED_VERTEX_PATH, Backend::OpenGLDriver::ShaderType::VERTEX);
		auto fragmentShader = mDriver->loadShader(DEFERRED_FRAGMENT_PATH, Backend::OpenGLDriver::ShaderType::FRAGMENT);
		auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::loadPbrShader()
	{
		auto vertexShader = mDriver->loadShader(LIGHTING_VERTEX_PATH, Backend::OpenGLDriver::ShaderType::VERTEX);
		auto fragmentShader = mDriver->loadShader(LIGHTING_FRAGMENT_PATH, Backend::OpenGLDriver::ShaderType::FRAGMENT);
		auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::loadSkyboxShader()
	{
		auto vertexShader = mDriver->loadShader(SKYBOX_VERTEX_PATH, Backend::OpenGLDriver::ShaderType::VERTEX);
		auto fragmentShader = mDriver->loadShader(SKYBOX_FRAGMENT_PATH, Backend::OpenGLDriver::ShaderType::FRAGMENT);
		auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::loadSkyboxRenderShader()
	{
		auto vertexShader = mDriver->loadShader(SKYBOX_RENDER_VERTEX_PATH, Backend::OpenGLDriver::ShaderType::VERTEX);
		auto fragmentShader = mDriver->loadShader(SKYBOX_RENDER_FRAGMENT_PATH, Backend::OpenGLDriver::ShaderType::FRAGMENT);
		auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::enable(Shader* shader)
	{
		mDriver->useShaderProgram(shader->getShaderProgram());
		mActiveShader = shader;
	}

	void ShaderRegistry::bindGlobalBufferToAll(const char* name, unsigned int bindIndex)
	{
		for (auto& shader : mShaderTable)
		{
			mDriver->registerUniformBlock(name, &shader, bindIndex);
		}
	}

	void ShaderRegistry::createShader(unsigned int program, const Backend::OpenGLDriver * driver)
	{
		mShaderTable.push_back(Shader{ program, driver });
	}
}