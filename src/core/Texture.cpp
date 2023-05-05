#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>

Texture::Texture(const char* path)
{
	auto img = stbi_load(path, &mWidth, &mHeight, &mCompression, 0);



	stbi_image_free(img);
}

void Texture::free()
{
	// free img
}
