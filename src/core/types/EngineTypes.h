#pragma once

#ifndef MINIENGINE_TYPES
#define MINIENGINE_TYPES

#include <cstdint>
#include "../utils/Color.h"

struct EngineInitParams
{
	uint16_t screenWidth;
	uint16_t screenHeight;
	Color clearColor;
};

#endif