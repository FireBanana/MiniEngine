#pragma once

#include <initializer_list>
#include "Material.h"

class Renderable
{
public:

	struct Builder
	{
		Builder() {};

		Builder& addVertices(std::initializer_list<float>&& vertices);
		Builder& addIndices(std::initializer_list<float>&& vertices);
		Builder& addMaterial(Material& material);
	};
};