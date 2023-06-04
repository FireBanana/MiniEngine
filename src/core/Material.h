#pragma once

#include "Shader.h"
#include "utils/ComponentArray.h"
#include "Texture.h"

class Engine;
class Entity;
class Shader;

namespace MiniEngine
{
	// =========================
	// Diffuse    - Location 1
	// Normal     - Location 2
	// Roughness  - Location 3
	// =========================
	class Material
	{
	public:

		Material(Shader* shader);

		void addTexture(int index, Texture texture);

		inline ComponentArray<Texture, 3> getTextureReference() const { return mTextureReference; }
		inline Shader* getShader() const { return mShader; }

	private:

		ComponentArray<Texture, 3> mTextureReference;
		Shader* mShader;

	};
}