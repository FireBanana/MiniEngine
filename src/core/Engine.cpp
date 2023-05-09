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

Mesh Engine::loadMesh(const char* path)
{
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err, wrn;

	RenderableComponent rComponent{};

	loader.LoadBinaryFromFile(&model, &err, &wrn, path);
	
	for (auto& bufferView : model.bufferViews)
	{
		if (bufferView.target == 0) continue; // Not element array

		auto& buffer = model.buffers[bufferView.buffer];

		rComponent.buffer = 
			{ 
				buffer.data.begin() + bufferView.byteOffset/8,
				buffer.data.begin() + bufferView.byteOffset/8 + bufferView.byteLength/8
			};

		// glBufferData
		// bufferView.byteLength
		// &buffer.data.at(0) + bufferView.byteOffset
	}

	for (auto& primitive : model.meshes[0].primitives)
	{
		auto& indexAccesor = model.accessors[primitive.indices];

		for (auto& attrib : primitive.attributes)
		{
			auto& accessor = model.accessors[attrib.second];

			auto byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
			auto size = accessor.type == TINYGLTF_TYPE_SCALAR ? 1 : accessor.type;

			if (attrib.first != "POSITION") continue; //Position only currently

			auto& indexBufferView = model.bufferViews[indexAccesor.bufferView];
			auto& indexBuffer = model.buffers[indexBufferView.buffer];
			
			rComponent.attributes = 
			{
				indexBuffer.data.begin() + indexBufferView.byteOffset/8,
				indexBuffer.data.begin() + indexBufferView.byteOffset/8 + indexBufferView.byteLength/8
			};

			// glEnableVertexAttribArray(attrib.second);
			//glVertexAttribPointer(attrib.second, size, accessor.componentType,
			//	accessor.normalized ? GL_TRUE : GL_FALSE,
			//	byteStride, BUFFER_OFFSET(accessor.byteOffset));
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
	}

	return{};
}


