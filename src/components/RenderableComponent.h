#pragma once

#include <vector>
#include "Component.h"
#include "../core/utils/ComponentArray.h"
#include "../core/Texture.h"
#include "../core/Shader.h"

struct RenderableComponent : public Component
{
	std::vector<float>		   buffer;
	std::vector<unsigned int>  indices;
	std::vector<unsigned int>  attributes;
	ComponentArray<Texture, 3> textures;
	Shader					   shader;
	Vector3					   worldPosition;

#ifdef USING_OPENGL
	unsigned int			  vaoId;
	unsigned int			  stride;
#endif
};