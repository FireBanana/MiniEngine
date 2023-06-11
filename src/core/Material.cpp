#include "Material.h"
#include "Texture.h"

namespace MiniEngine
{
	Material::Creator& Material::Creator::addTexture(TextureType textureType, Texture texture)
	{
		mTextureReference.set((int)textureType, texture);
		return *this;
	}
	
	Material::Creator& Material::Creator::addShader(Shader* shader)
	{
		mShader = shader;
		return *this;
	}
	
	Material::Creator& Material::Creator::addMaterialProperty(PropertyType propertyType, float value)
	{
		mMaterialProperties.set((int)propertyType, value);
	}
	MaterialInstance Material::Creator::create()
	{
		return { mShader, std::move(mTextureReference), std::move(mMaterialProperties) };
	}
}