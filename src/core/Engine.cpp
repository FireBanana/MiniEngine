#include "Engine.h"
#include "Entity.h"
#include "Renderable.h"
#include "Scene.h"
#include "model-loader.h"

#include <tiny_gltf.h>
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
	int w, h, c;
	auto img = stbi_load(path, &w, &h, &c, 0);
	auto id = mGlPlatform.get()->getDriver()->createTexture(w, h, img);

	stbi_image_free(img);
	return { w, h, c, id };
}

RenderableComponent* Engine::loadMeshToRenderable(const char* path, Scene* scene)
{
	auto results = ModelLoader::ModelLoader::load(path);

	for (auto& i : results.models)
	{
		auto rComponent = Renderable::Builder()
			.addBufferData(std::move(i.bufferData))
			.addIndices(std::move(i.indices))
			.addBufferAttributes(std::move(i.vertexAttributeSizes))
			.build(scene, scene->createEntity());
	}

	return {};
}


