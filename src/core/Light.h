#pragma once
#include <glm.hpp>

enum LIGHT_TYPE
{
	Point		= 0x0,
	Direction   = 0x1
};

struct Light
{
	glm::vec3  m_Position;
	LIGHT_TYPE m_LightType;
};