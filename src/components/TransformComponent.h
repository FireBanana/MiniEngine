#pragma once

#include "Component.h"

namespace MiniEngine::Components
{
	struct TransformComponent : public Component
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};
}