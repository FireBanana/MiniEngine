#include "Loader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>
#include <iostream>
#include <glm.hpp>

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
			std::vector<float> normalData;
			std::vector<float> tangentData;

			std::vector<unsigned int> indices;

			auto primitive = object.primitives[0];

			if (primitive.mode != TINYGLTF_MODE_TRIANGLES) throw;

			// TODO: Cleanup ifs
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

					auto& buffer = model.buffers[bufferView.buffer];

					for (auto i = 0; i < size; ++i)
					{
						vertexData.push_back(
							*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride + bufferView.byteOffset)])))
						);

						vertexData.push_back(
							*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float)])))
						);

						vertexData.push_back(
							*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float) * 2])))
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

					auto& buffer = model.buffers[bufferView.buffer];

					for (auto i = 0; i < size; ++i)
					{
						uvData.push_back(
							*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride + bufferView.byteOffset)])))
						);

						uvData.push_back(
							*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float)])))
						);
					}
				}
				else if (attrib.first == "NORMAL")
				{
					auto& accessor = model.accessors[attrib.second];
					auto& bufferView = model.bufferViews[accessor.bufferView];
					auto type = accessor.type;

					auto byteStride = accessor.ByteStride(bufferView);
					auto size = accessor.count;

					auto& buffer = model.buffers[bufferView.buffer];

					for (auto i = 0; i < size; ++i)
					{
						normalData.push_back(
							*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride + bufferView.byteOffset)])))
						);

						normalData.push_back(
							*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float)])))
						);

						normalData.push_back(
							*(reinterpret_cast<float*>(&(buffer.data[(i * byteStride + bufferView.byteOffset) + sizeof(float) * 2])))
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

			//std::cout << "indices: " << indices.size() << ", vertices: " << vertexData.size() << std::endl;
			//int max = 0;
			//for (int i = 0; i < indices.size(); ++i) if (indices[i] > max) max = indices[i];
			//std::cout << max << std::endl;

			// Calculate Tangents manually
			for (int i = 0; i < indices.size() - 3; i += 3)
			{
				auto vert1 = glm::vec3{ vertexData[indices[i] * 3], vertexData[indices[i] * 3 + 1], vertexData[indices[i] * 3 + 2] };
				auto vert2 = glm::vec3{ vertexData[indices[i + 1] * 3], vertexData[indices[i + 1] * 3 + 1], vertexData[indices[i + 1] * 3 + 2] };
				auto vert3 = glm::vec3{ vertexData[indices[i + 2] * 3], vertexData[indices[i + 2] * 3 + 1], vertexData[indices[i + 2] * 3 + 2] };

				auto _uv1 = glm::vec2{ uvData[indices[i] * 2], uvData[indices[i] * 2 + 1] };
				auto _uv2 = glm::vec2{ uvData[indices[i + 1] * 2], uvData[indices[i + 1] * 2 + 1] };
				auto _uv3 = glm::vec2{ uvData[indices[i + 2] * 2], uvData[indices[i + 2] * 2 + 1] };

				auto uv1 = _uv2 - _uv1;
				auto uv2 = _uv3 - _uv1;

				auto edge1 = vert2 - vert1;
				auto edge2 = vert3 - vert1;

				auto d = 1.0f / (uv1.x * uv2.y - uv2.x * uv1.y);

				auto x = d * (uv2.y * edge1.x - uv1.y * edge2.x);
				auto y = d * (uv2.y * edge1.y - uv1.y * edge2.y);
				auto z = d * (uv2.y * edge1.z - uv1.y * edge2.z);

				tangentData.push_back(x);
				tangentData.push_back(y);
				tangentData.push_back(z);
			}

			auto bufferData = std::vector<float>();
			auto it_vert = vertexData.begin();
			auto it_uv = uvData.begin();
			auto it_normal = normalData.begin();
			auto it_tangent = tangentData.begin();

			int vCount = 0;
			glm::vec3 vert1, vert2, vert3;
			glm::vec2 uv1, uv2, uv3;

			while (it_vert != vertexData.end() && it_uv != uvData.end() && it_normal != normalData.end() && it_tangent != tangentData.end())
			{
				if (vCount % 3 == 0) //new vertex
				{

				}

				auto vert1 = *(it_vert++);
				auto vert2 = *(it_vert++);
				auto vert3 = *(it_vert++);

				auto uv1 = *(it_uv++);
				auto uv2 = *(it_uv++);

				bufferData.push_back(vert1);
				bufferData.push_back(vert2);
				bufferData.push_back(vert3);

				bufferData.push_back(uv1);
				bufferData.push_back(uv2);

				bufferData.push_back(*(it_normal++));
				bufferData.push_back(*(it_normal++));
				bufferData.push_back(*(it_normal++));

				bufferData.push_back(*(it_tangent++));
				bufferData.push_back(*(it_tangent++));
				bufferData.push_back(*(it_tangent++));

				++vCount;
			}

			results.models.push_back({ std::move(bufferData), std::move(indices), {3, 2, 3, 3} });
		}

		return results;
	}

	ImageLoaderResults ImageLoader::load(const char* path, bool isFloat, bool flipImage)
	{
		int w, h, c;
		void* data;

		stbi_set_flip_vertically_on_load(flipImage);

		if (isFloat)
			data = stbi_loadf(path, &w, &h, &c, 0);
		else
			data = stbi_load(path, &w, &h, &c, 0);

		if (data == nullptr) std::cout << stbi_failure_reason() << std::endl;

		return { w, h, c, isFloat, data };
	}
}