#include "Engine.h"
#include <memory>

Engine::Engine(const EngineInitParams& params)
{
	mGlPlatform = std::make_unique<OpenGLPlatform>(params);
}

void Engine::execute()
{
	mGlPlatform->execute();
}
