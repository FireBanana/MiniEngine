#pragma once

#include <vector>
#include "Mesh.h"
#include "Camera.h"
#include "../components/MeshComponent.h"
#include "../components/CameraComponent.h"
#include "../components/TransformComponent.h"

class Entity;

class Scene
{
public:

	Scene(Engine* engine);

	Entity* createEntity();

	MeshComponent* createMesh(const Mesh::Builder* builderResults, Entity* entity);
	CameraComponent* createCamera(const Camera::Builder* builderResults, Entity* entity);

	void setCameraActive(const CameraComponent* camera);

	inline const std::vector<MeshComponent>* getMeshComponentDatabase() const { return &mMeshComponentDatabase; }
	inline const std::vector<CameraComponent>* getCameraComponentDatabase() const { return &mCameraComponentDatabase; }

private:

	Engine* mEngine;

	std::vector<Entity>				mEntityDatabase;
	std::vector<CameraComponent>	mCameraComponentDatabase;
	std::vector<MeshComponent>		mMeshComponentDatabase;
	std::vector<TransformComponent> mTransformComponentDatabase;
};