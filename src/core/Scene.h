#pragma once

#include <vector>
#include "Mesh.h"
#include "Camera.h"
#include "../components/MeshComponent.h"
#include "../components/CameraComponent.h"

class Entity;

class Scene
{
public:

	Scene();

	Entity* createEntity();

	void createMesh(const Mesh::Builder* builderResults, Entity* entity);
	void createCamera(const Camera::Builder* builderResults, Entity* entity);

	inline const std::vector<MeshComponent>* getMeshComponentDatabase() const { return &mMeshComponentDatabase; }
	inline const std::vector<CameraComponent>* getCameraComponentDatabase() const { return &mCameraComponentDatabase; }

private:

	std::vector<Entity> mEntityDatabase;
	std::vector<CameraComponent> mCameraComponentDatabase;
	std::vector<MeshComponent> mMeshComponentDatabase;
};