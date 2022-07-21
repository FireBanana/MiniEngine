#include "Renderer.h"

void Renderer::AddObject(Object& o)
{
	m_ObjectList.emplace_back(o);
}

void Renderer::AddLight(Light& l)
{
	m_LightList.emplace_back(l);
}

void Renderer::Render(const Camera& cam) const
{
	for (auto& obj : m_ObjectList)
		obj.get().Draw(cam);
}

void Renderer::Render(const Camera& cam, Shader& shader) const
{
	for (auto& obj : m_ObjectList)
		obj.get().Draw(cam, shader);
}
