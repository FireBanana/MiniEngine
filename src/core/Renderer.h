#pragma once

#include<vector>
#include"Object.h"
#include"Camera.h"
#include "Light.h"

class Renderer
{
public:

	Renderer() = default;

	void AddObject(Object& o);
	void AddLight(Light& l);
	void Render(const Camera& cam) const;
	void Render(const Camera& cam, Shader& shader) const;

private:

	std::vector<std::reference_wrapper<Object>> m_ObjectList;
	std::vector<std::reference_wrapper<Light>> m_LightList;
};

