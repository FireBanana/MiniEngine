#pragma once

#ifndef MINIENGINE_TYPES_H
#define MINIENGINE_TYPES_H

#include <glm/glm.hpp>
#include <cstdint>
#include "Color.h"

using Matrix4x4 = glm::mat4;

struct EngineInitParams
{
	uint16_t screenWidth;
	uint16_t screenHeight;
	Color clearColor;
};

struct Vector3
{
public:

	float x, y, z;

};

#endif