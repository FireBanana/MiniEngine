#pragma once

#include "../core/Shader.h"
#include "../core/utils/ComponentArray.h"

namespace MiniEngine
{
	struct MaterialInstance
	{
		Shader* shader;
		ComponentArray<Texture, 3> textureReference;
		ComponentArray<float, 16> materialProperties;
	};
}