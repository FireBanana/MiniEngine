#include "Engine.h"
#include "Entity.h"
#include "Renderable.h"
#include "Scene.h"
#include "Loader.h"
#include <memory>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Engine::Engine(const EngineInitParams& params)
{
	mGlPlatform = std::make_unique<OpenGLPlatform>(params);
	mShaderRegistry = std::make_unique<ShaderRegistry>(mGlPlatform->getDriver());

	mShaderRegistry->loadDeferredShader();
	mShaderRegistry->enable(mShaderRegistry->getDeferredShader());
}

void Engine::execute(Scene* scene)
{
	mGlPlatform->execute(scene);
}

Texture Engine::loadTexture(const char* path)
{
	auto results = MiniTools::ImageLoader::load(path);
	auto id = mGlPlatform.get()->getDriver()->createTexture(results.width, results.height, results.data);

	return { results.width, results.height, 0, id };
}

RenderableComponent* Engine::loadMeshToRenderable(const char* path, Scene* scene)
{
	auto results = MiniTools::ModelLoader::load(path);
	auto texture = loadTexture("C:\\Users\\Owais\\Desktop\\img.png");
	Material material{ texture, getShaderRegistry()->getDeferredShader() };

	for (auto& i : results.models)
	{
		auto rComponent = Renderable::Builder()
			.addBufferData(std::move(i.bufferData))
			.addIndices(std::move(i.indices))
			.addBufferAttributes(std::move(i.vertexAttributeSizes))
			.addMaterial(&material)
			.build(scene, scene->createEntity());
	}

	return {};
}


