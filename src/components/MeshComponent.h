#pragma once

#include <vector>
#include "Component.h"

class MeshComponent : public Component
{
public:

	std::vector<float>		  buffer;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> attributes;
	unsigned int			  stride;
};