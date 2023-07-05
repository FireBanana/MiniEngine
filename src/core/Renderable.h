#pragma once
#include <vector>

namespace MiniEngine
{
	class Engine;
	class Entity;
	class Scene;
	struct MaterialInstance;

	namespace Components
	{
		struct RenderableComponent;
	}

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

			Builder() : mMaterialInstance(nullptr) { }
			Builder(const Builder&) = delete;
			Builder operator=(const Builder&) = delete;
			Builder(const Builder&&) = delete;
			Builder operator=(const Builder&&) = delete;

			Builder& addBufferData(std::vector<float>&& buffer);
			Builder& addBufferAttributes(std::vector<unsigned int>&& attributes);
			Builder& addIndices(std::vector<unsigned int>&& indices);
			Builder& addMaterialInstance(MaterialInstance* MaterialInstance);
			Builder& addModel(const char* path);
			Builder& isLit(bool isLit);

			MiniEngine::Components::RenderableComponent* build(Scene* scene, Entity* entity);

			inline const std::vector<float>& getBuffer() const { return mBuffer; }
			inline const std::vector<unsigned int>& getIndices() const { return mIndices; }
			inline const std::vector<unsigned int>& getAttributes() const { return mAttributes; }
			inline const char* getModelPath() const { return mModelPath; }
			inline MaterialInstance* getMaterialInstance() const { return mMaterialInstance; }

		private:

			std::vector<float>		  mBuffer;
			std::vector<unsigned int> mIndices;
			std::vector<unsigned int> mAttributes;
			MaterialInstance* mMaterialInstance;
			bool					  mIsLit;
			const char* mModelPath;
		};

	};
}