#pragma once

#include <vector>
#include "../core/types/EngineTypes.h"
#include "Component.h"

class CameraComponent : public Component
{
public:

	Vector3 position;
	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
};