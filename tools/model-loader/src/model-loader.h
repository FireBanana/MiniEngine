#pragma once

#include <vector>

namespace ModelLoader
{

	struct Model
	{
		std::vector<float>		  bufferData;
		std::vector<unsigned int> indices;
		std::vector<unsigned int> vertexAttributeSizes;
	};

	struct TextureData
	{
		int width;
		int height;
		float* data;
	};

	struct ModelLoaderResults
	{
		std::vector<Model> models;
		TextureData		   mainTexture;
	};

	class ModelLoader
	{
	public:
		static ModelLoaderResults load(const char* path);
	};

}