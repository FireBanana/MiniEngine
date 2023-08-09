#pragma once

#include "Shader.h"
#include "ComponentArray.h"
#include "MaterialInstance.h"
#include "Texture.h"
#include "EnumExtension.h"
#include "EngineConstants.h"
#include "EngineTypes.h"

namespace MiniEngine
{
	class Engine;
	class Entity;
	class Shader;

	// =========================
	// Texture References:
	// Diffuse    - Location 0
	// Normal     - Location 1
	// Roughness  - Location 2
	// =========================
	class Material
	{
	public:

		Material() = delete;
		Material(const Material&) = delete;
		Material(const Material&&) = delete;
		Material operator=(const Material&) = delete;
		Material operator=(const Material&&) = delete;

		// Should be used if texture not specified
		enum class PropertyType : int
		{
			Roughness = 0,
			Metallic = 1,
		};

		class Creator
		{
		public:

			Creator() : mTextureMask(0) {  };
			Creator(const Creator&) = delete;
			Creator operator=(const Creator&) = delete;
			Creator(const Creator&&) = delete;
			Creator operator=(const Creator&&) = delete;

			Creator& addTexture(MiniEngine::Types::TextureType textureType, Texture texture);
			Creator& addShader(Shader* shader);
			Creator& addMaterialProperty(PropertyType propertyType, float value);

			MaterialInstance create();

			bool isTextureSet(MiniEngine::Types::TextureType type);

			inline ComponentArray<Texture, MiniEngine::MAX_TEXTURE_TYPES> getTextureReference() const { return mTextureReference; }
			inline ComponentArray<float, 16> getMaterialPropertyReference() const { return mMaterialProperties; }
			inline Shader* getShader() const { return mShader; }

		private:

			ComponentArray<Texture, MiniEngine::MAX_TEXTURE_TYPES> mTextureReference;
			ComponentArray<float, 16> mMaterialProperties;
			uint32_t mTextureMask;
			Shader* mShader;
		};

	};
}