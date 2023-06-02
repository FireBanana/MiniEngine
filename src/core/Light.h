#pragma once
#include "types/EngineTypes.h"

namespace MiniEngine
{
	class Scene;
	class Entity;
	class LightComponent;

	class Light
	{
	public:

		Light() = delete;
		Light(const Light&) = delete;
		Light(const Light&&) = delete;
		Light operator=(const Light&) = delete;
		Light operator=(const Light&&) = delete;

		class Builder
		{

		public:
			Builder();
			Builder(const Builder&) = delete;
			Builder operator=(const Builder&) = delete;
			Builder(const Builder&&) = delete;
			Builder operator=(const Builder&&) = delete;

			Builder& setPosition(Vector3 position);
			Builder& setIntensity(float intensity);

			LightComponent* build(Scene* scene, Entity* entity);

		private:

			Vector3 mPosition;
			float mIntensity;
		};
	};
}