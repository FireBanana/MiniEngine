#include "Engine.h"
#include "Entity.h"
#include "Renderable.h"
#include "Scene.h"
#include "Loader.h"
#include <memory>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Engine::Engine(const EngineInitParams& params)
{
	mGlPlatform = std::make_unique<OpenGLPlatform>(params, this);
	mShaderRegistry = std::make_unique<ShaderRegistry>(mGlPlatform->getDriver());

	mShaderRegistry->loadDeferredShader();
	mShaderRegistry->loadPbrShader();
	mShaderRegistry->enable(mShaderRegistry->getDeferredShader());
}

void Engine::execute(Scene* scene)
{
	mGlPlatform->execute(scene);
}

Texture Engine::loadTexture(const char* path)
{
	auto results = MiniTools::ImageLoader::load(path);
	auto id = mGlPlatform.get()->getDriver()->createTexture(results.width, results.height, results.channels, results.data);

	return { results.width, results.height, results.channels, id };
}


