#pragma once

#include<vector>
#include"Object.h"
#include"Camera.h"
#include "Light.h"
#include "ShadowMapper.h"

class Renderer
{
public:

	Renderer();

	void AddObject(Object& o);
	void AddLight(Light& l);
	void Render(Camera cam);
	void Render(Camera cam, Shader& shader);
    void ShadowPass(Renderer *renderer, Camera *camera, Light *light);

private:

	std::vector<Object*> m_ObjectList;
	std::vector<Light*>  m_LightList;

	ShadowMapper		 m_ShadowMapper;
};

