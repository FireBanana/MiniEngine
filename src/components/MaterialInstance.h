#pragma once

#include "ComponentArray.h"
#include "EngineConstants.h"
#include "Texture.h"
#include "core/Shader.h"
#include <array>

namespace MiniEngine
{
	struct MaterialInstance
	{
		uint32_t textureMask;
        Shader *shader;
        std::array<Texture, MiniEngine::MAX_TEXTURE_TYPES> textureReference;
        std::array<float, 16> materialProperties;
	};
}
