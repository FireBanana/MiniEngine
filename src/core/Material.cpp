#include "Material.h"
#include "Texture.h"

namespace MiniEngine
{
	Material::Material(Shader* shader) :
		mShader(shader)
	{

	}

	void Material::addTexture(int index, Texture texture)
	{
		mTextureReference.set(index, texture);
	}
}