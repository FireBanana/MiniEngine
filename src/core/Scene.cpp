#include "Scene.h"
#include "../components/MeshComponent.h"
#include "../components/CameraComponent.h"
#include "Entity.h"

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

void Scene::createCamera(const Camera::Builder* builderResults, Entity* entity)
{
	CameraComponent c{};
	c.entityHandle = entity;
	c.position = builderResults->getPosition();
	c.aspectRatio = builderResults->getAspectRatio();
	c.farPlane = builderResults->getFarPlane();
	c.fov = builderResults->getFov();
	c.nearPlane = builderResults->getNearPlane();

	mCameraComponentDatabase.push_back(std::move(c));
}
