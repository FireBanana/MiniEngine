#pragma once

#include "types/EngineTypes.h"

class Scene;
class Entity;
class CameraComponent;

class Camera
{
public:

	Camera() = delete;
	Camera(Camera&) = delete;
	Camera operator=(Camera&) = delete;
	Camera(Camera&&) = default;

	class Builder
	{
	public:

		Builder() { }
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		Builder& setPosition(Vector3 position);
		Builder& setFOV(float fov);
		Builder& setAspectRatio(float ratio);
		Builder& setNearFarPlane(float near, float far);

		CameraComponent* build(Scene* scene, Entity* entity);

		inline const Vector3 getPosition() const { return mPosition; }
		inline const float getFov() const { return mFov; }
		inline const float getAspectRatio() const { return mAspectRatio; }
		inline const float getNearPlane() const { return mNearPlane; }
		inline const float getFarPlane() const { return mFarPlane; }

	private:

		Vector3 mPosition;
		float mFov;
		float mAspectRatio;
		float mNearPlane;
		float mFarPlane;		
	};
};