#pragma once
#include "Entity.h"
#include <vector>

class Engine;

class Mesh
{
public:

	Mesh() = delete;
	Mesh(const Mesh&) = delete;
	Mesh operator=(const Mesh&) = delete;
	Mesh(const Mesh&&) = delete;
	Mesh operator=(const Mesh&&) = delete;

	class Builder
	{
	public:

		Builder() { /* Create in mesh reginstry or something */ mMesh = {}; }
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		Builder& addVertices(std::initializer_list<float>&& vertices);
		Builder& addIndices(std::initializer_list<float>&& vertices);

		Mesh* build(Engine* engine, Entity* entity);

	private:

		Mesh* mMesh;
	};

private:

	Entity* mEntity;

	std::vector<float> mVertices;
	std::vector<int> mIndices;
};