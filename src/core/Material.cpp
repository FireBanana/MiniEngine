#include "Material.h"
#include "Texture.h"

namespace MiniEngine
{
	Material::Creator& Material::Creator::addTexture(MiniEngine::Types::TextureType textureType, Texture texture)
	{
		mTextureMask |= (0x1 << (int)textureType);
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
		return *this;
	}

	MaterialInstance Material::Creator::create()
	{
		return { mTextureMask, mShader, std::move(mTextureReference), std::move(mMaterialProperties) };
	}

	bool Material::Creator::isTextureSet(MiniEngine::Types::TextureType type)
	{
		return (mTextureMask >> (int)type) & 0x1;
	}
}