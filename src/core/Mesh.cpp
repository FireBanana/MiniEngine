#include "Mesh.h"
#include "Engine.h"
#include "Scene.h"

Mesh::Builder& Mesh::Builder::addBufferData(std::initializer_list<float>&& buffer)
{
	mBuffer = std::move(buffer);
	return *this;
}

Mesh::Builder& Mesh::Builder::addBufferAttributes(std::initializer_list<unsigned int>&& attributes)
{
	mAttributes = std::move(attributes);
	return *this;
}

Mesh::Builder& Mesh::Builder::addIndices(std::initializer_list<unsigned int>&& indices)
{
	mIndices = std::move(indices);
	return *this;
}

Mesh::Builder& Mesh::Builder::isLit(bool isLit)
{
	mIsLit = isLit;
	return *this;
}

MeshComponent* Mesh::Builder::build(Scene* scene, Entity* entity)
{
	return scene->createMesh(this, entity);
}
