#include "Texture.h"
#include <glad/glad.h>

Texture::Texture(int width, int height, int compression, unsigned int id) :
	mWidth(width), mHeight(height), mCompression(compression), mId(id)
{
}
