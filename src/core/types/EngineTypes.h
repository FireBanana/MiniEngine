#pragma once

#ifndef MINIENGINE_TYPES_H
#define MINIENGINE_TYPES_H

#include <glm/glm.hpp>
#include <cstdint>
#include "Color.h"

namespace MiniEngine
{
namespace Types
{

	enum class TextureType : int
	{
		Diffuse = 0,
		Normal = 1,
		Roughness = 2
	};

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
}
}

using Vector3 = MiniEngine::Types::Vector3;
using Matrix4x4 = glm::mat4;

#endif