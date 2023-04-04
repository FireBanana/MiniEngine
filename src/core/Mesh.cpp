#include "Mesh.h"
#include "Engine.h"

Mesh::Builder& Mesh::Builder::addVertices(std::initializer_list<float>&& vertices)
{
	mVertices = std::move(vertices);
	return *this;
}

Mesh::Builder& Mesh::Builder::addIndices(std::initializer_list<unsigned int>&& indices)
{
	mIndices = std::move(indices);
	return *this;
}

void Mesh::Builder::build(Engine* engine, Entity* entity)
{
	auto details = BuilderResults{ this->mVertices, this->mIndices };
	engine->createMesh(details, entity);
}
