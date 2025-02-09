#pragma once

#include "Shader.h"
#include "MaterialInstance.h"
#include "Texture.h"
#include "EngineTypes.h"
#include <array>

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

			Creator();
			Creator(const Creator&) = delete;
			Creator operator=(const Creator&) = delete;
			Creator(const Creator&&) = delete;
			Creator operator=(const Creator&&) = delete;

			Creator& addTexture(MiniEngine::Types::TextureType textureType, Texture texture);
			Creator& addShader(Shader* shader);
			Creator& addMaterialProperty(PropertyType propertyType, float value);

			MaterialInstance create();

			bool isTextureSet(MiniEngine::Types::TextureType type);

			inline std::array<Texture, 32> getTextureReference() const { return mTextureReference; }
			inline std::array<float, 16> getMaterialPropertyReference() const { return mMaterialProperties; }
			inline Shader* getShader() const { return mShader; }

		private:

			std::array<Texture, 32> mTextureReference;
			std::array<float, 16> mMaterialProperties;
			uint32_t mTextureMask;
			Shader* mShader;
		};

	};
}
