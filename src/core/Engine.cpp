#include "Engine.h"
#include "Entity.h"
#include "Renderable.h"

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

RenderableComponent* Engine::loadMeshToRenderable(const char* path, Scene* scene, Entity* entity)
{
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err, wrn;

	loader.LoadBinaryFromFile(&model, &err, &wrn, path);

	std::vector<float> bufferData;
	std::vector<unsigned int> indices;

	auto primitive = model.meshes[0].primitives[0];

	for (auto& attrib : primitive.attributes)
	{
		if (attrib.first != "POSITION") continue; //Position only currently
		auto& accessor = model.accessors[attrib.second];
		auto& bufferView = model.bufferViews[accessor.bufferView];
		auto type = accessor.type;

		auto byteStride = accessor.ByteStride(bufferView);
		auto size = accessor.count;

		auto& positionBuffer = model.buffers[bufferView.buffer];

		for (auto i = 0; i < size; ++i)
		{
			bufferData.push_back(
				*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset)])))
			);

			bufferData.push_back(
				*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float)]))));

			bufferData.push_back(
				*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float) * 2])))
			);
		}
	}

	// Get indices
	auto& indexAccessor = model.accessors[primitive.indices];
	auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
	auto indexStride = indexAccessor.ByteStride(indexBufferView);
	auto& indexBuffer = model.buffers[indexBufferView.buffer];

	for (auto i = 0; i < indexAccessor.count; ++i)
	{
		auto val = *(reinterpret_cast<unsigned int*>(&(indexBuffer.data[i * indexStride + indexBufferView.byteOffset]))) & ((~0u) >> 16);

		indices.push_back(val);
	}

	//if (model.textures.size() > 0)
	//{
	//	auto& tex = model.textures[0];

	//	if (tex.source > -1)
	//	{
	//		auto& image = model.images[tex.source];

	//		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
	//		//	format, type, &image.image.at(0));
	//	}
	//}

	auto rComponent = Renderable::Builder()
		.addBufferData(std::move(bufferData))
		.addIndices(std::move(indices))
		.addBufferAttributes({ 3 })
		.build(scene, entity);

		return rComponent;
}


