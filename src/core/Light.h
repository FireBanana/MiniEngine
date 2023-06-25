#pragma once
#include "types/EngineTypes.h"

namespace MiniEngine
{
	class Scene;
	class Entity;

	namespace Components
	{
		struct LightComponent;
	}	

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
			Builder() {};
			Builder(const Builder&) = delete;
			Builder operator=(const Builder&) = delete;
			Builder(const Builder&&) = delete;
			Builder operator=(const Builder&&) = delete;

			Builder& setPosition(Vector3 position);
			Builder& setIntensity(float intensity);

			inline const Vector3 getPosition() const { return mPosition; }
			inline const float getIntensity() const { return mIntensity; }

			Components::LightComponent* build(Scene* scene, Entity* entity);

		private:

			Vector3 mPosition;
			float mIntensity;
		};
	};
}