#include "Camera.h"

Camera::Camera() :
	mPosition(Vector3{ 0.0f, 0.0f, 0.0f }),
	mFov(90.0f), mAspect(1.0f), mNear(0.0f), mFar(100.0f)
{
	
}