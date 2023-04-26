#pragma once
#include <vector>

class Engine;
class Entity;
class Scene;
class MeshComponent;

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

		Builder() { }
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		Builder& addBufferData(std::initializer_list<float>&& buffer);
		Builder& addBufferAttributes(std::initializer_list<unsigned int>&& attributes);
		Builder& addIndices(std::initializer_list<unsigned int>&& indices);
		Builder& isLit(bool isLit);

		MeshComponent* build(Scene* scene, Entity* entity);

		inline const std::vector<float>& getBuffer() const { return mBuffer; }
		inline const std::vector<unsigned int>& getIndices() const { return mIndices; }
		inline const std::vector<unsigned int>& getAttributes() const { return mAttributes; }

	private:

		std::vector<float> mBuffer;
		std::vector<unsigned int> mIndices;
		std::vector<unsigned int> mAttributes;
		bool mIsLit;
	};

};