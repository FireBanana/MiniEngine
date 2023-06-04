#pragma once

#include <vector>
#include "../core/types/EngineTypes.h"
#include "Component.h"

namespace MiniEngine::Components
{
	struct CameraComponent : public Component
	{
		Vector3 position; //move to transform component
		float fov;
		float aspectRatio;
		float nearPlane;
		float farPlane;
	};
}