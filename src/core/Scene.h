#pragma once

#include <vector>
#include "Renderable.h"
#include "Light.h"
#include "Camera.h"
#include "Skybox.h"
#include "LightComponent.h"
#include "RenderableComponent.h"
#include "SkyboxComponent.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
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
		Components::SkyboxComponent* createSkybox(const Skybox::Builder* builderResults, Entity* entity);

		void setCameraActive(const Components::CameraComponent* camera);
		void addLight(const Components::LightComponent* light);
		void toggleSkyBox(bool enable);

		inline const auto& getRenderableComponentDatabase() const { return mRenderableComponentDatabase; }
		inline const auto& getCameraComponentDatabase() const { return mCameraComponentDatabase; }
		inline const auto& getSkyBox() const { return mActiveSkybox; }

	private:

		Engine* mEngine;
		Components::SkyboxComponent mActiveSkybox;

		ComponentArray<Entity, MAX_COMPONENT_SIZE>							mEntityDatabase;
		ComponentArray<Components::CameraComponent, MAX_COMPONENT_SIZE>	    mCameraComponentDatabase;
		ComponentArray<Components::RenderableComponent, MAX_COMPONENT_SIZE> mRenderableComponentDatabase;
		ComponentArray<Components::TransformComponent, MAX_COMPONENT_SIZE>  mTransformComponentDatabase;
		ComponentArray<Components::LightComponent, MAX_COMPONENT_SIZE>		mLightComponentDatabase;
	};

}