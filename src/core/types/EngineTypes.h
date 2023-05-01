#pragma once

#ifndef MINIENGINE_TYPES_H
#define MINIENGINE_TYPES_H

#include <cstdint>
#include "../utils/Color.h"

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