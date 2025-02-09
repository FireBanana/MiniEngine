#pragma once

#include "Texture.h"
#include "core/Shader.h"
#include <array>

namespace MiniEngine
{
	struct MaterialInstance
	{
		uint32_t textureMask;
        Shader *shader;
        std::array<Texture, 32> textureReference;
        std::array<float, 16> materialProperties;
	};
}
