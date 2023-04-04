#pragma once

#include <vector>
#include "Component.h"

class MeshComponent : public Component
{
public:

	std::vector<float>	  vertices;
	std::vector<unsigned> indices;

};