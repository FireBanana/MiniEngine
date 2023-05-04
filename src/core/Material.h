#pragma once

#include "Shader.h"

class Engine;
class Entity;
class Shader;
class Texture;

class Material
{
public:

	Material() = delete;
	Material(const Material&) = delete;
	Material operator=(const Material&) = delete;
	Material(const Material&&) = delete;
	Material operator=(const Material&&) = delete;

	class Builder
	{
	public:

		Builder() { }
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		Builder& setTexture(Texture* texture);
		Builder& setFloat(float value);

		Material* build( Engine* engine, Entity* entity );

	private:

		std::vector<Texture*> mTextureReference;

	};
};