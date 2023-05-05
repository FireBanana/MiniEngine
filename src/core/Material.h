#pragma once

#include "Shader.h"
#include "utils/ComponentArray.h"
#include "Texture.h"

class Engine;
class Entity;
class Shader;

class Material
{
public:

	Material( Texture texture, Shader* shader );

	inline ComponentArray<Texture, 32> getTextureReference() const { return mTextureReference; }
	inline Shader* getShader() const { return mShader; }

private:

	ComponentArray<Texture, 32> mTextureReference;
	Shader* mShader;

};