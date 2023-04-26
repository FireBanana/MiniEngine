#pragma once

#include "Component.h"

class TransformComponent : public Component
{
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
};