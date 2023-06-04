#include "Light.h"
#include "Scene.h"

namespace MiniEngine
{
	Light::Builder& Light::Builder::setPosition(Vector3 position)
	{
		return *this;
	}

	Light::Builder& Light::Builder::setIntensity(float intensity)
	{
		return *this;
	}

	Components::LightComponent* Light::Builder::build(Scene* scene, Entity* entity)
	{
		return scene->createLight(this, entity);
	}
}

