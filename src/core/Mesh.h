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

		Builder(Mesh& m) : mMesh(m) {}
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		Builder& addVertices(std::initializer_list<float>&& vertices);
		Builder& addIndices(std::initializer_list<float>&& vertices);

		Mesh& build();

	private:

		Mesh& mMesh;
	};

private:

	Entity* mEntity;

	std::vector<float> mVertices;
	std::vector<int> mIndices;
};