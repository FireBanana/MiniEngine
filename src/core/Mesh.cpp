#include "Mesh.h"

Mesh::Builder& Mesh::Builder::addVertices(std::initializer_list<float>&& vertices)
{
	return *this;
}

Mesh::Builder& Mesh::Builder::addIndices(std::initializer_list<float>&& vertices)
{
	return *this;
}

Mesh* Mesh::Builder::build(Engine* engine, Entity* entity)
{
	return mMesh;
}
