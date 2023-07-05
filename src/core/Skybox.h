#pragma once
#include "EngineTypes.h"
#include "Texture.h"

namespace MiniEngine
{
	class Scene;
	class Entity;

	namespace Components
	{
		struct SkyboxComponent;
	}

	class Skybox
	{
	public:

		Skybox() = delete;
		Skybox(const Skybox&) = delete;
		Skybox(const Skybox&&) = delete;
		Skybox operator=(const Skybox&) = delete;
		Skybox operator=(const Skybox&&) = delete;

		enum class SkyboxType
		{
			Default,
			Skybox
		};

		class Builder
		{

		public:
			Builder() {};
			Builder(const Builder&) = delete;
			Builder operator=(const Builder&) = delete;
			Builder(const Builder&&) = delete;
			Builder operator=(const Builder&&) = delete;

			Builder& setTexture(Texture tex);

			inline const Texture getTexture() const { return mTexture; }

			Components::SkyboxComponent build(Scene* scene, Entity* entity);

		private:

			Texture mTexture;
		};
	};
}