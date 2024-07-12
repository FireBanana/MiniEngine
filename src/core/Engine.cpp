#include "Engine.h"
#include "Entity.h"
#include "Loader.h"
#include "Logger.h"
#include "Renderable.h"
#include "Scene.h"
#include "VulkanPlatform.h"
#include <memory>

namespace MiniEngine
{
	Engine::Engine(MiniEngine::Types::EngineInitParams& params)
	{
		MiniEngine::Logger::print("Initializing engine...");

        mGraphicsPlatform = std::make_unique<Backend::VulkanPlatform>();
		
		mGraphicsPlatform->initialize(params, this);

		mShaderRegistry = std::make_unique<ShaderRegistry>(mGraphicsPlatform->getDriver());
		mGlobalBufferRegistry = std::make_unique<GlobalBufferRegistry>(mGraphicsPlatform->getDriver());

		mGlobalBufferRegistry->createNewBinding(GlobalBufferRegistry::BlockType::SceneBlock, 0);

		mShaderRegistry->loadDeferredShader();
		mShaderRegistry->loadPbrShader();
		mShaderRegistry->loadSkyboxShader();
		mShaderRegistry->loadSkyboxRenderShader();
		mShaderRegistry->loadSkyboxConvoluter();
		mShaderRegistry->loadEnvPrefilterShader();
		mShaderRegistry->loadEnvPreComputeBrdfShader();
		mShaderRegistry->loadPostProcessShader();

		createDefaultMaterial();
	}

	void Engine::execute(Scene* scene)
	{
		mGraphicsPlatform->execute(scene);
	}

	Texture Engine::loadTexture(const char* path, Texture::TextureType type, bool flipYAxis)
	{
		auto results = MiniTools::ImageLoader::load(path, type == Texture::TextureType::CubeMap, flipYAxis);

        auto id = mGraphicsPlatform.get()
                      ->getDriver()
                      ->createTexture(results.width,
                                      results.height,
                                      results.channels,
                                      results.data,
                                      (Backend::VulkanDriver::TextureType) type);

        return { results.width, results.height, results.channels, id };
	}

	void Engine::addSlider(const char* name, float* value, float min, float max, std::function<void()> cb)
	{
		mGraphicsPlatform->getUiInterface()->createSliderPanel(name, value, min, max, cb);
	}

	void Engine::addCheckbox(const char* name, bool& flag, std::function<void()> cb)
	{
		mGraphicsPlatform->getUiInterface()->createBooleanPanel(name, flag, cb);
	}

	void Engine::createDefaultMaterial()
	{
		mDefaultMaterial = MiniEngine::Material::Creator()
			.addShader(getShaderRegistry()->getDeferredShader())
			.addMaterialProperty(MiniEngine::Material::PropertyType::Roughness, 0.15f)
			.addMaterialProperty(MiniEngine::Material::PropertyType::Metallic, 1.f)
			.create();
	}
}



