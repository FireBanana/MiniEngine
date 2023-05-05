#pragma once

#include <vector>
#include "Component.h"
#include "../core/utils/ComponentArray.h"
#include "../core/Texture.h"
#include "../core/Shader.h"

struct RenderableComponent : public Component
{
	std::vector<float>			buffer;
	std::vector<unsigned int>	indices;
	std::vector<unsigned int>	attributes;
	ComponentArray<Texture, 32> textures;
	Shader						shader;

#ifdef USING_OPENGL
	unsigned int			  vaoId;
	unsigned int			  stride;
#endif
};