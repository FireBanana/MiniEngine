#pragma once

#include "Component.h"
#include "../core/types/EngineTypes.h"

namespace MiniEngine::Components
{
	class LightComponent : public Component
	{
	public:
		Vector3 position; //Move to transform
		float intensity;
	};
}