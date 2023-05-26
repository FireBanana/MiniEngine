#include "Material.h"
#include "Texture.h"

Material::Material(Shader* shader) :
	mShader(shader)
{
	
}

void Material::addTexture(int index, Texture texture)
{
	mTextureReference.set(index, texture);
}
