#pragma once

#include <vector>
#include "Component.h"

class MeshComponent : public Component
{
public:

	std::vector<float>		  buffer;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> attributes;

#ifdef USING_OPENGL
	const unsigned int		  vaoId = 1;
	unsigned int			  vertexBufferId;
	unsigned int			  indexBufferId;
	unsigned int			  stride;
#endif
};