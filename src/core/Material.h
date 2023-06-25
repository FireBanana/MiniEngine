#pragma once

#include "Shader.h"
#include "utils/ComponentArray.h"
#include "MaterialInstance.h"
#include "Texture.h"

class Engine;
class Entity;
class Shader;

namespace MiniEngine
{
	// =========================
	// Texture References:
	// Diffuse    - Location 1
	// Normal     - Location 2
	// Roughness  - Location 3
	// =========================
	class Material
	{
	public:

		Material() = delete;
		Material(const Material&) = delete;
		Material(const Material&&) = delete;
		Material operator=(const Material&) = delete;
		Material operator=(const Material&&) = delete;

		enum class TextureType : int
		{
			Diffuse   = 0,
			Normal	  = 1,
			Roughness = 2
		};

		// Should be used if texture not specified
		enum class PropertyType : int
		{
			Roughness = 0,
			Metallic = 1,
		};

		class Creator
		{
		public:

			Creator() {};
			Creator(const Creator&) = delete;
			Creator operator=(const Creator&) = delete;
			Creator(const Creator&&) = delete;
			Creator operator=(const Creator&&) = delete;

			Creator& addTexture(TextureType textureType, Texture texture);
			Creator& addShader(Shader* shader);
			Creator& addMaterialProperty(PropertyType propertyType, float value);

			MaterialInstance create();

			inline ComponentArray<Texture, 3> getTextureReference() const { return mTextureReference; }
			inline ComponentArray<float, 16> getMaterialPropertyReference() const { return mMaterialProperties; }
			inline Shader* getShader() const { return mShader; }

		private:

			ComponentArray<Texture, 3> mTextureReference;
			ComponentArray<float, 16> mMaterialProperties;
			Shader* mShader;
		};

	};
}