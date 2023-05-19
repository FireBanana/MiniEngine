#include "model-loader.h"

#include <tiny_gltf.h>
#include <iostream>

namespace ModelLoader
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
			std::vector<float> bufferData;
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
						bufferData.push_back(
							*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset)])))
						);

						bufferData.push_back(
							*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float)])))
						);

						bufferData.push_back(
							*(reinterpret_cast<float*>(&(positionBuffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float) * 2])))
						);
					}
				}
				else if (attrib.first == "TEXCOORD_0")
				{

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

			results.models.push_back({ std::move(bufferData), std::move(indices), {3} });
		}

		return results;
	}
}