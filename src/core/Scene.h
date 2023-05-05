#pragma once

#include <vector>
#include "Renderable.h"
#include "Camera.h"
#include "../components/RenderableComponent.h"
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

	RenderableComponent* createRenderable(const Renderable::Builder* builderResults, Entity* entity);
	CameraComponent* createCamera(const Camera::Builder* builderResults, Entity* entity);

	void setCameraActive(const CameraComponent* camera);

	inline const auto getRenderableComponentDatabase() const { return mRenderableComponentDatabase; }
	inline const auto getCameraComponentDatabase() const { return mCameraComponentDatabase; }

private:

	Engine* mEngine;

	ComponentArray<Entity, MAX_COMPONENT_SIZE>			    mEntityDatabase;
	ComponentArray<CameraComponent, MAX_COMPONENT_SIZE>	    mCameraComponentDatabase;
	ComponentArray<RenderableComponent, MAX_COMPONENT_SIZE> mRenderableComponentDatabase;
	ComponentArray<TransformComponent, MAX_COMPONENT_SIZE>  mTransformComponentDatabase;
};