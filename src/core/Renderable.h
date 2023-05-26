#pragma once
#include <vector>

class Engine;
class Entity;
class Scene;
class RenderableComponent;
class Material;

class Renderable
{
public:

	Renderable() = delete;
	Renderable(const Renderable&) = delete;
	Renderable operator=(const Renderable&) = delete;
	Renderable(const Renderable&&) = delete;
	Renderable operator=(const Renderable&&) = delete;

	class Builder
	{
	public:

		Builder() : mMaterial(nullptr) { }
		Builder(const Builder&) = delete;
		Builder operator=(const Builder&) = delete;
		Builder(const Builder&&) = delete;
		Builder operator=(const Builder&&) = delete;

		Builder& addBufferData(std::vector<float>&& buffer);
		Builder& addBufferAttributes(std::vector<unsigned int>&& attributes);
		Builder& addIndices(std::vector<unsigned int>&& indices);
		Builder& addMaterial(Material* material);
		Builder& addModel(const char* path);
		Builder& isLit(bool isLit);

		RenderableComponent* build(Scene* scene, Entity* entity);

		inline const std::vector<float>& getBuffer() const { return mBuffer; }
		inline const std::vector<unsigned int>& getIndices() const { return mIndices; }
		inline const std::vector<unsigned int>& getAttributes() const { return mAttributes; }
		inline const Material* getMaterial() const { return mMaterial; }
		inline const char* getModelPath() const { return mModelPath; }

	private:

		std::vector<float>		  mBuffer;
		std::vector<unsigned int> mIndices;
		std::vector<unsigned int> mAttributes;
		Material*				  mMaterial;
		bool					  mIsLit;
		const char*				  mModelPath;
	};

};