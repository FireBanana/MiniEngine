#pragma once
#include "Entity.h"
#include <vector>

class Mesh
{
public:

	Mesh();

	class Builder
	{
	public:

		Builder() = delete;
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		static void addVertices();
		static void addIndices();
		static void assignVertexArrayObject();
	};

private:

	Entity* mEntity;

	std::vector<float> mVertices;
	std::vector<int> mIndices;
};