#pragma once

#include "Shader.h"

class Material
{
public:

	Material(const Shader& shader);
	inline const Shader& getShader() const noexcept { return mShader; }

	void setCullMode();
	void setTexture();
	void setColor();
	void setFloat();

private:

	//Entity* mEntity;
	Shader mShader;

};