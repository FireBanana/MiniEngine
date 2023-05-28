#pragma once

#include <vector>

namespace MiniTools
{
	// TODO: Add unloader for all

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

	struct ImageLoaderResults
	{
		int width;
		int height;
		int channels;
		unsigned char* data;
	};

	class ModelLoader
	{
	public:
		static ModelLoaderResults load(const char* path);
	};

	class ImageLoader
	{
	public:
		static ImageLoaderResults load(const char* path);
	};
}