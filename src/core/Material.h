#pragma once

#include "Shader.h"

class Engine;
class Entity;
class Shader;

class Material
{
public:

	Material() = delete;
	Material(const Material&) = delete;
	Material operator=(const Material&) = delete;
	Material(const Material&&) = delete;
	Material operator=(const Material&&) = delete;

	inline const Shader* getShader() const noexcept { return mShader; }

	class Builder
	{
	public:

		Builder() { }
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		Material* build( Engine* engine, Entity* entity );

	private:

	};

	void setCullMode();
	void setTexture();
	void setColor();
	void setFloat();

private:

	//Entity* mEntity;
	Shader* mShader;  // move data out of decl

};