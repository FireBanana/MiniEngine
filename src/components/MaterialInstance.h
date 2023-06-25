#pragma once

#include "Shader.h"
#include "Texture.h"
#include "ComponentArray.h"

namespace MiniEngine
{
	struct MaterialInstance
	{
		Shader* shader;
		ComponentArray<Texture, 3> textureReference;
		ComponentArray<float, 16> materialProperties;
	};
}