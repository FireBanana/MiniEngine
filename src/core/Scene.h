#pragma once

#include <vector>
#include "Mesh.h"
#include "Camera.h"
#include "../components/MeshComponent.h"
#include "../components/CameraComponent.h"
#include "../components/TransformComponent.h"
#include "Entity.h"
#include "utils/Span.h"
#include "utils/ComponentArray.h"

constexpr size_t MAX_COMPONENT_SIZE = 256;

class Scene
{
public:

	Scene(Engine* engine);

	Entity* createEntity();

	MeshComponent* createMesh(const Mesh::Builder* builderResults, Entity* entity);
	CameraComponent* createCamera(const Camera::Builder* builderResults, Entity* entity);

	void setCameraActive(const CameraComponent* camera);

	inline const auto getMeshComponentDatabase() const { return mMeshComponentDatabase; }
	inline const auto getCameraComponentDatabase() const { return mCameraComponentDatabase; }

private:

	Engine* mEngine;

	ComponentArray<Entity, MAX_COMPONENT_SIZE>			   mEntityDatabase;
	ComponentArray<CameraComponent, MAX_COMPONENT_SIZE>	   mCameraComponentDatabase;
	ComponentArray<MeshComponent, MAX_COMPONENT_SIZE>	   mMeshComponentDatabase;
	ComponentArray<TransformComponent, MAX_COMPONENT_SIZE> mTransformComponentDatabase;
};