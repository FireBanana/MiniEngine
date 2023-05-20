#include "Loader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>
#include <iostream>

namespace MiniTools
{
	ModelLoaderResults ModelLoader::load(const char* path)
	{
		tinygltf::TinyGLTF loader;
		tinygltf::Model model;
		std::string err, wrn;

		ModelLoaderResults results{};

		loader.LoadBinaryFromFile(&model, &err, &wrn, path);

		for (auto& object : model.meshes)
		{
			std::vector<float> vertexData;
			std::vector<float> uvData;
			std::vector<unsigned int> indices;

			auto primitive = object.primitives[0];

			if (primitive.mode != TINYGLTF_MODE_TRIANGLES) throw;

			// Get vertices
			for (auto& attrib : primitive.attributes)
			{
				if (attrib.first == "POSITION")
				{
					auto& accessor = model.accessors[attrib.second];
					auto& bufferView = model.bufferViews[accessor.bufferView];
					auto type = accessor.type;

					auto byteStride = accessor.ByteStride(bufferView);
					auto size = accessor.count;

					auto& positionBuffer = model.buffers[bufferView.buffer];

					for (auto i = 0; i < size; ++i)
					{
						vertexData.push_back(
							*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset)])))
						);

						vertexData.push_back(
							*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float)])))
						);

						vertexData.push_back(
							*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float) * 2])))
						);
					}
				}
				else if (attrib.first == "TEXCOORD_0")
				{
					auto& accessor = model.accessors[attrib.second];
					auto& bufferView = model.bufferViews[accessor.bufferView];
					auto type = accessor.type;

					auto byteStride = accessor.ByteStride(bufferView);
					auto size = accessor.count;

					auto& positionBuffer = model.buffers[bufferView.buffer];

					for (auto i = 0; i < size; ++i)
					{
						uvData.push_back(
							*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset)])))
						);

						uvData.push_back(
							*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float)])))
						);
					}
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

			// Get image
			auto& imgView = model.bufferViews[model.images[0].bufferView];
			auto& imgBuffer = model.buffers[imgView.buffer];

			int w, h, comp;
			auto f = stbi_loadf_from_memory(&(*(imgBuffer.data.begin() + imgView.byteOffset)), imgView.byteLength, &w, &h, &comp, 0);

			if (f == nullptr)
			{
				std::cout << stbi_failure_reason();
				throw;
			}

			auto bufferData = std::vector<float>();
			auto it_vert = vertexData.begin();
			auto it_uv = uvData.begin();

			while (it_vert != vertexData.end() || it_uv != uvData.end())
			{
				bufferData.push_back(*(it_vert++));
				bufferData.push_back(*(it_vert++));
				bufferData.push_back(*(it_vert++));

				bufferData.push_back(*(it_uv++));
				bufferData.push_back(*(it_uv++));
			}

			results.models.push_back({ std::move(bufferData), std::move(indices), {3, 2} });
		}

		return results;
	}
	
	ImageLoaderResults ImageLoader::load(const char* path)
	{
		int w, h, c;
		auto img = stbi_load(path, &w, &h, &c, 0);

		return { w, h, img };
	}
}