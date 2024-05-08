#include "ShaderRegistry.h"
#include "core/Shader.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MiniEngine
{
ShaderRegistry::ShaderRegistry(Backend::VulkanDriver *driver)
    : mDriver(driver)
    , mShaderTable()
{}

void ShaderRegistry::loadDeferredShader()
{
    auto vertexShader = mDriver->loadShader(DEFERRED_VERTEX_PATH,
                                            Backend::VulkanDriver::ShaderType::VERTEX);
    auto fragmentShader = mDriver->loadShader(DEFERRED_FRAGMENT_PATH,
                                              Backend::VulkanDriver::ShaderType::FRAGMENT);
    auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

    createShader(shader, mDriver);
	}

	void ShaderRegistry::loadPbrShader()
	{
        auto vertexShader = mDriver->loadShader(LIGHTING_VERTEX_PATH,
                                                Backend::VulkanDriver::ShaderType::VERTEX);
        auto fragmentShader = mDriver->loadShader(LIGHTING_FRAGMENT_PATH,
                                                  Backend::VulkanDriver::ShaderType::FRAGMENT);
        auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::loadSkyboxShader()
	{
        auto vertexShader = mDriver->loadShader(SKYBOX_VERTEX_PATH,
                                                Backend::VulkanDriver::ShaderType::VERTEX);
        auto fragmentShader = mDriver->loadShader(SKYBOX_FRAGMENT_PATH,
                                                  Backend::VulkanDriver::ShaderType::FRAGMENT);
        auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::loadSkyboxRenderShader()
	{
        auto vertexShader = mDriver->loadShader(SKYBOX_RENDER_VERTEX_PATH,
                                                Backend::VulkanDriver::ShaderType::VERTEX);
        auto fragmentShader = mDriver->loadShader(SKYBOX_RENDER_FRAGMENT_PATH,
                                                  Backend::VulkanDriver::ShaderType::FRAGMENT);
        auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::loadSkyboxConvoluter()
	{
        auto vertexShader = mDriver->loadShader(SKYBOX_VERTEX_PATH,
                                                Backend::VulkanDriver::ShaderType::VERTEX);
        auto fragmentShader = mDriver->loadShader(ENV_CONVOLUTION_PATH,
                                                  Backend::VulkanDriver::ShaderType::FRAGMENT);
        auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::loadEnvPrefilterShader()
	{
        auto vertexShader = mDriver->loadShader(SKYBOX_VERTEX_PATH,
                                                Backend::VulkanDriver::ShaderType::VERTEX);
        auto fragmentShader = mDriver->loadShader(ENV_PREFILTER_PATH,
                                                  Backend::VulkanDriver::ShaderType::FRAGMENT);
        auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::loadEnvPreComputeBrdfShader()
	{
        auto vertexShader = mDriver->loadShader(LIGHTING_VERTEX_PATH,
                                                Backend::VulkanDriver::ShaderType::VERTEX);
        auto fragmentShader = mDriver->loadShader(ENV_PRECOMPUTE_BRDF_PATH,
                                                  Backend::VulkanDriver::ShaderType::FRAGMENT);
        auto shader = mDriver->createShaderProgram(vertexShader, fragmentShader);

		createShader(shader, mDriver);
	}

	void ShaderRegistry::loadPostProcessShader()
	{
        auto vertexShader = mDriver->loadShader(LIGHTING_VERTEX_PATH,
                                                Backend::VulkanDriver::ShaderType::VERTEX);
        auto fragmentShader = mDriver->loadShader(POST_PROCESS_FRAGMENT_PATH,
                                                  Backend::VulkanDriver::ShaderType::FRAGMENT);
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

    void ShaderRegistry::createShader(unsigned int program, Backend::VulkanDriver *driver)
    {
        mShaderTable.push_back(MiniEngine::Shader{program, driver});
    }
}
