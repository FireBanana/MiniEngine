#include "Engine.h"
#include "Entity.h"
#include <memory>

Engine::Engine(const EngineInitParams& params)
{
	mGlPlatform = std::make_unique<OpenGLPlatform>(params);
	mShaderRegistry = std::make_unique<ShaderRegistry>(mGlPlatform->getDriver());

	mShaderRegistry->loadDeferredShader();
	mShaderRegistry->getDeferredShader()->enable();
}

void Engine::execute()
{
	mGlPlatform->execute(this);
}

Entity* Engine::createEntity()
{
	mEntityDatabase.push_back(Entity{});
	return &(mEntityDatabase.back());
}

void Engine::createMesh(const Mesh::BuilderResults& builderResults, Entity* entity)
{
	MeshComponent m{};
	m.entityHandle = entity;
	m.vertices = builderResults.mVertices;
	m.indices = builderResults.mIndices;

	mMeshComponentDatabase.push_back(std::move(m));
}
