#include "Engine.h"
#include "Entity.h"
#include "Renderable.h"
#include "Scene.h"
#include "Loader.h"
#include <memory>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace MiniEngine
{
	Engine::Engine(const EngineInitParams& params)
	{
		mGlPlatform = std::make_unique<Backend::OpenGLPlatform>(params, this);
		mShaderRegistry = std::make_unique<ShaderRegistry>(mGlPlatform->getDriver());
		mGlobalBufferRegistry = std::make_unique<GlobalBufferRegistry>(mGlPlatform->getDriver());

		mGlobalBufferRegistry->createNewBinding(GlobalBufferRegistry::BlockType::SceneBlock, 0);

		mShaderRegistry->loadDeferredShader();
		mShaderRegistry->loadPbrShader();
		mShaderRegistry->loadSkyboxShader();
		mShaderRegistry->loadSkyboxRenderShader();
		mShaderRegistry->loadSkyboxConvoluter();
	}

	void Engine::execute(Scene* scene)
	{
		mGlPlatform->execute(scene);
	}

	Texture Engine::loadTexture(const char* path, Texture::TextureType type, bool flipYAxis)
	{
		auto results = MiniTools::ImageLoader::load(path, type == Texture::TextureType::CubeMap, flipYAxis);

		auto id = mGlPlatform.get()->getDriver()->createTexture(results.width, results.height, results.channels, results.data, type);

		return { results.width, results.height, results.channels, id };
	}

	void Engine::addSlider(const char* name, float* value, float min, float max, std::function<void()> cb)
	{
		mGlPlatform->getUiInterface()->createSliderPanel(name, value, min, max, cb);
	}
}


