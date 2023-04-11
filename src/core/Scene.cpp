#include "Scene.h"
#include "../components/MeshComponent.h"
#include "Entity.h"
#include "Camera.h"

Scene::Scene()
{

}

Entity* Scene::createEntity()
{
	mEntityDatabase.push_back(Entity{});
	return &(mEntityDatabase.back());
}

void Scene::createMesh(const Mesh::Builder* builderResults, Entity* entity)
{
	MeshComponent m{};
	m.entityHandle = entity;
	m.buffer = builderResults->getBuffer();
	m.indices = builderResults->getIndices();
	m.attributes = builderResults->getAttributes();
	
	unsigned int stride = 0;

	for (auto i = 0; i < m.attributes.size(); ++i)
	{
		stride += m.attributes[i];
	}

	m.stride = stride;

	mMeshComponentDatabase.push_back(std::move(m));
}

void Scene::createCamera(Entity* entity)
{
	Camera c{};
	mCameraComponentDatabase.push_back(std::move(c));
}
