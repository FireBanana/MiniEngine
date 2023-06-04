#pragma once

#include <vector>
#include "Renderable.h"
#include "Light.h"
#include "Camera.h"
#include "../components/LightComponent.h"
#include "../components/RenderableComponent.h"
#include "../components/CameraComponent.h"
#include "../components/TransformComponent.h"
#include "Entity.h"
#include "utils/Span.h"
#include "utils/ComponentArray.h"

constexpr size_t MAX_COMPONENT_SIZE = 256;

namespace MiniEngine
{
	class Scene
	{
	public:

		Scene(Engine* engine);

		Entity* createEntity();

		Components::RenderableComponent* createRenderable(const Renderable::Builder* builderResults, Entity* entity);
		Components::CameraComponent* createCamera(const Camera::Builder* builderResults, Entity* entity);
		Components::LightComponent* createLight(const Light::Builder* builderResults, Entity* entity);

		void setCameraActive(const Components::CameraComponent* camera);

		inline const auto& getRenderableComponentDatabase() const { return mRenderableComponentDatabase; }
		inline const auto& getCameraComponentDatabase() const { return mCameraComponentDatabase; }

	private:

		Engine* mEngine;

		ComponentArray<Entity, MAX_COMPONENT_SIZE>							mEntityDatabase;
		ComponentArray<Components::CameraComponent, MAX_COMPONENT_SIZE>	    mCameraComponentDatabase;
		ComponentArray<Components::RenderableComponent, MAX_COMPONENT_SIZE> mRenderableComponentDatabase;
		ComponentArray<Components::TransformComponent, MAX_COMPONENT_SIZE>  mTransformComponentDatabase;
	};

}