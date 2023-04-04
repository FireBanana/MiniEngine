#pragma once
#include <vector>

class Engine;
class Entity;

class Mesh
{
public:

	Mesh() = delete;
	Mesh(const Mesh&) = delete;
	Mesh operator=(const Mesh&) = delete;
	Mesh(const Mesh&&) = delete;
	Mesh operator=(const Mesh&&) = delete;

	class BuilderResults
	{
	public:

		BuilderResults(std::vector<float>& verts, std::vector<unsigned int> indices) : 
			mVertices(std::move(verts)), mIndices(std::move(indices))
		{}

		std::vector<float> mVertices;
		std::vector<unsigned int> mIndices;
	};

	class Builder
	{
	public:

		Builder() { }
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		Builder& addVertices(std::initializer_list<float>&& vertices);
		Builder& addIndices(std::initializer_list<unsigned int>&& indices);

		void build(Engine* engine, Entity* entity);

	private:

		std::vector<float> mVertices;
		std::vector<unsigned int> mIndices;

	};

private:

	Entity* mEntity;

	std::vector<float> mVertices;
	std::vector<int> mIndices;
};