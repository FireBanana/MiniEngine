#pragma once

#include <vector>
#include "Mesh.h"

class Entity;
class MeshComponent;
class Camera;

class Scene
{
public:

	Scene();

	Entity* createEntity();

	void createMesh(const Mesh::Builder* builderResults, Entity* entity);
	void createCamera(Entity* entity);

	inline const std::vector<MeshComponent>* getMeshComponentDatabase() const { return &mMeshComponentDatabase; }

private:

	std::vector<Entity> mEntityDatabase;
	std::vector<MeshComponent> mMeshComponentDatabase;
	std::vector<Camera> mCameraComponentDatabase; // Change to Camera Component
};