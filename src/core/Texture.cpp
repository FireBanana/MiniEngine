#include "Texture.h"
#include <glad/glad.h>

namespace MiniEngine
{
	Texture::Texture(int width, int height, int channels, unsigned int id) :
		mWidth(width), mHeight(height), mChannels(channels), mId(id)
	{
	}
}