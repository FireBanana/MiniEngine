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
	}

	void Engine::execute(Scene* scene)
	{
		mGlPlatform->execute(scene);
	}

	Texture Engine::loadTexture(const char* path, Texture::TextureType type)
	{
		auto results = MiniTools::ImageLoader::load(path, type == Texture::TextureType::CubeMap);

		auto id = type == Texture::TextureType::Default ?
			mGlPlatform.get()->getDriver()->createTexture(results.width, results.height, results.channels, results.data) :
			mGlPlatform.get()->getDriver()->createCubeMap(results.width, results.height, results.channels, results.data);

		return { results.width, results.height, results.channels, id };
	}
}


