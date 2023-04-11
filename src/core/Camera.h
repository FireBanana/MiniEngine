#pragma once

#include "types/EngineTypes.h"

class Camera
{
public:

	Camera();
	Camera(Camera&) = delete;
	Camera operator=(Camera&) = delete;

private:

	Vector3 mPosition;
	float mFov;
	float mAspect;
	float mNear;
	float mFar;

};