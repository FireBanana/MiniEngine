#include "Texture.h"
#include <glad/glad.h>

namespace MiniEngine
{
unsigned int Texture::sResourceIdCounter = 0;

Texture::Texture(int width, int height, int channels)
    : mWidth(width)
    , mHeight(height)
    , mChannels(channels)
{
    mId = sResourceIdCounter++;
}
}
