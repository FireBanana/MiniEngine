#pragma once

#include "Shader.h"

class Material
{
public:

	Material(const Shader& shader);
	inline const Shader& getShader() const noexcept { return mShader; }

private:

	//Entity* mEntity;
	Shader mShader;

};