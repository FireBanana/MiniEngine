#include "Loader.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <glm.hpp>

namespace MiniTools
{
	ModelLoaderResults ModelLoader::load(const char* path)
	{
		Assimp::Importer importer{};
		ModelLoaderResults res{};

		const auto scene = importer.ReadFile(path, aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_FlipUVs);

		if (scene == nullptr) {
			std::cout << "\nLoading file failed!\n";
			std::cout << importer.GetErrorString();
			return {};
		}

		if (scene->mNumMeshes > 1) {
			std::cout << "\nLoading file failed! Only 1 mesh supported currently\n";
			//return {};
		}

		auto mesh = scene->mMeshes[0];

		res.models.push_back({});

		for (int i = 0; i < mesh->mNumVertices; ++i) {
			res.models[0].bufferData.push_back(mesh->mVertices[i].x);
			res.models[0].bufferData.push_back(mesh->mVertices[i].y);
			res.models[0].bufferData.push_back(mesh->mVertices[i].z);

			res.models[0].bufferData.push_back(mesh->mNormals[i].x);
			res.models[0].bufferData.push_back(mesh->mNormals[i].y);
			res.models[0].bufferData.push_back(mesh->mNormals[i].z);

			res.models[0].bufferData.push_back(mesh->mTextureCoords[0][i].x);
			res.models[0].bufferData.push_back(mesh->mTextureCoords[0][i].y);
		}

		for (int i = 0; i < mesh->mNumFaces; ++i) {
			for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j) {
				res.models[0].indices.push_back(mesh->mFaces[i].mIndices[j]);
			}
		}
		
		res.models[0].vertexAttributeSizes = { 3, 3, 2 };

		return res;
	}

	ImageLoaderResults ImageLoader::load(const char* path, bool isFloat, bool flipImage)
	{
		int w, h, c;
		void* data;

		stbi_set_flip_vertically_on_load(flipImage);

		if (isFloat)
			data = stbi_loadf(path, &w, &h, &c, STBI_rgb_alpha);
		else
			data = stbi_load(path, &w, &h, &c, STBI_rgb_alpha);

		if (data == nullptr) std::cout << stbi_failure_reason() << std::endl;

		return { w, h, c, isFloat, data };
	}
}