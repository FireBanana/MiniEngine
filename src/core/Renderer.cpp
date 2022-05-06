#include "Renderer.h"

Renderer::Renderer()
{

}

void Renderer::AddObject(Object& o)
{
	m_ObjectList.push_back(&o);
}

void Renderer::AddLight(Light& l)
{
	m_LightList.push_back(&l);
}

void Renderer::Render(Camera cam)
{
	for (auto& obj : m_ObjectList)
		obj->Draw(cam);
}
