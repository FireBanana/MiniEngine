#pragma once

#include "Shader.h"
#include "Texture.h"
#include "ComponentArray.h"
#include "EngineConstants.h"

namespace MiniEngine
{
	struct MaterialInstance
	{
		uint32_t textureMask;
		Shader* shader;
		ComponentArray<Texture, MiniEngine::MAX_TEXTURE_TYPES> textureReference;
		ComponentArray<float, 16> materialProperties;
	};
}