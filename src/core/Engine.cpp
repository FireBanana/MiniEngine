#include "Engine.h"
#include "Entity.h"

#include <tiny_gltf.h>
#include <memory>

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

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

RenderableComponent Engine::loadMesh(const char* path)
{
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err, wrn;

	RenderableComponent rComponent{};

	loader.LoadBinaryFromFile(&model, &err, &wrn, path);

	auto primitive = model.meshes[0].primitives[0];

	for (auto& attrib : primitive.attributes)
	{
		if (attrib.first != "POSITION") continue; //Position only currently
		auto& accessor = model.accessors[attrib.second];
		auto& bufferView = model.bufferViews[accessor.bufferView];
		auto type = accessor.type;

		auto byteStride = accessor.ByteStride(bufferView);
		auto offset = BUFFER_OFFSET(accessor.byteOffset);
		auto size = accessor.count;
		auto& buffer = model.buffers[bufferView.buffer];

		auto section = std::vector<Vector3>();

		for (auto i = 0; i < size; ++i)
		{
			section.push_back(
				{
					*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride)]))),
					*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride) + sizeof(float)]))),
					*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride) + sizeof(float) * 2])))
				});
		}
	}

	if (model.textures.size() > 0)
	{
		auto& tex = model.textures[0];

		if (tex.source > -1)
		{
			auto& image = model.images[tex.source];

			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
			//	format, type, &image.image.at(0));
		}
	}
	

	return rComponent;
}


