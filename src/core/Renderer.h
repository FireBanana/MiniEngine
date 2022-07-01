#pragma once

#include<vector>
#include"Object.h"
#include"Camera.h"
#include "Light.h"

class Renderer
{
public:

	Renderer();

	void AddObject(Object& o);
	void AddLight(Light& l);
	void Render(Camera cam);
	void Render(Camera cam, Shader& shader);

private:

	std::vector<Object*> m_ObjectList;
	std::vector<Light*> m_LightList;
};

