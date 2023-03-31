#include "Engine.h"
#include <memory>

Engine::Engine(const EngineInitParams& params)
{
	mGlPlatform = std::make_unique<OpenGLPlatform>(params);
	mShaderRegistry = std::make_unique<ShaderRegistry>(mGlPlatform->getDriver());

	mShaderRegistry->loadDeferredShader();
}

void Engine::execute()
{
	mGlPlatform->execute();
}

Entity* Engine::createEntity()
{
	return nullptr;
}
