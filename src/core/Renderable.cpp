#include "Renderable.h"
#include "Engine.h"
#include "Scene.h"

Renderable::Builder& Renderable::Builder::addBufferData(std::vector<float>&& buffer)
{
	mBuffer = std::forward<std::vector<float>&&>(buffer);
	return *this;
}

Renderable::Builder& Renderable::Builder::addBufferAttributes(std::vector<unsigned int>&& attributes)
{
	mAttributes = std::forward<std::vector<unsigned int>&&>(attributes);
	return *this;
}

Renderable::Builder& Renderable::Builder::addIndices(std::vector<unsigned int>&& indices)
{
	mIndices = std::forward<std::vector<unsigned int>&&>(indices);
	return *this;
}

Renderable::Builder& Renderable::Builder::addMaterial(Material* material)
{
	mMaterial = material;
	return *this;
}

Renderable::Builder& Renderable::Builder::isLit(bool isLit)
{
	mIsLit = isLit;
	return *this;
}

RenderableComponent* Renderable::Builder::build(Scene* scene, Entity* entity)
{
	return scene->createRenderable(this, entity);
}
