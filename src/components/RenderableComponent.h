#pragma once

#include <vector>
#include "Component.h"
#include "ComponentArray.h"
#include "Texture.h"
#include "Shader.h"
#include "MaterialInstance.h"

namespace MiniEngine::Components
{
	struct RenderableComponent : public Component
	{
		std::vector<float>		   buffer;
		std::vector<unsigned int>  indices;
		std::vector<unsigned int>  attributes;
		MaterialInstance*          materialInstance;
		Shader					   shader;
		Vector3					   worldPosition;
		Vector3					   rotation;
		unsigned int			  stride;
	};
}