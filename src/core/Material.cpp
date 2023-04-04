#include "Material.h"

Material::Builder& Material::Builder::setShader(const Shader* shader)
{
	return *this;
}

Material* Material::Builder::build(Engine* engine, Entity* entity)
{
	return {};
}