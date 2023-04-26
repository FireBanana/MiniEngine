#pragma once

#include "types/EngineTypes.h"

class TransformComponent;
class Scene;
class Entity;

class Transform
{
public:

	Transform() = delete;
	Transform(const Transform&) = delete;
	Transform operator=(const Transform&) = delete;
	Transform(const Transform&&) = delete;
	Transform operator=(const Transform&&) = delete;

	class Builder
	{
	public:

		Builder() { }
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		Builder& setPosition(Vector3 position);
		Builder& setRotation(Vector3 rotation);
		Builder& setScale(Vector3 scale);

		TransformComponent* build(Scene* scene, Entity* entity);

		inline Vector3 getPosition() const { return mPosition; }
		inline Vector3 getRotation() const { return mRotation; }
		inline Vector3 getScale() const { return mScale; }

	private:

		Vector3 mPosition;
		Vector3 mRotation;
		Vector3 mScale;
	};
};