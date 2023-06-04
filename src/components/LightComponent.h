#pragma once

#include "Component.h"
#include "../core/types/EngineTypes.h"

namespace MiniEngine::Components
{
	struct LightComponent : public Component
	{
		Vector3 position; //Move to transform
		float intensity;
	};
}