#pragma once

#include "Camera.h"
#include "CameraComponent.h"
#include "Entity.h"
#include "Light.h"
#include "LightComponent.h"
#include "Renderable.h"
#include "RenderableComponent.h"
#include "Skybox.h"
#include "SkyboxComponent.h"
#include "TransformComponent.h"
#include "utils/ComponentArray.h"
#include <vector>

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
		Components::SkyboxComponent createSkybox(const Skybox::Builder* builderResults, Entity* entity);

		void setCameraActive(const Components::CameraComponent* camera);
		void addLight(const Components::LightComponent* light);
		void setSkyboxActive(Components::SkyboxComponent* skybox);

		inline const auto& getRenderableComponentDatabase() const { return mRenderableComponentDatabase; }
		inline const auto& getCameraComponentDatabase() const { return mCameraComponentDatabase; }
		inline const auto& getSkyBox() const { return mActiveSkybox; }

	private:

		Engine* mEngine;
		Components::SkyboxComponent* mActiveSkybox;

		ComponentArray<Entity, MAX_COMPONENT_SIZE>							mEntityDatabase;
		ComponentArray<Components::CameraComponent, MAX_COMPONENT_SIZE>	    mCameraComponentDatabase;
		ComponentArray<Components::RenderableComponent, MAX_COMPONENT_SIZE> mRenderableComponentDatabase;
		ComponentArray<Components::TransformComponent, MAX_COMPONENT_SIZE>  mTransformComponentDatabase;
		ComponentArray<Components::LightComponent, MAX_COMPONENT_SIZE>		mLightComponentDatabase;
	};

}
