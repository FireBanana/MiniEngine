#include "Camera.h"
#include "Scene.h"

namespace MiniEngine
{
	Camera::Builder& Camera::Builder::setPosition(Vector3 position)
	{
		mPosition = position;
		return *this;
	}

	Camera::Builder& Camera::Builder::setFOV(float fov)
	{
		mFov = fov;
		return *this;
	}

	Camera::Builder& Camera::Builder::setAspectRatio(float ratio)
	{
		mAspectRatio = ratio;
		return *this;
	}

    Camera::Builder &Camera::Builder::setNearFarPlane(float _near, float _far)
    {
        mNearPlane = _near;
        mFarPlane = _far;
        return *this;
	}

	Components::CameraComponent* Camera::Builder::build(Scene* scene, Entity* entity)
	{
		return scene->createCamera(this, entity);
	}
}
