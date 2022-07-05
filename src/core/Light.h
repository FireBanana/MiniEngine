#pragma once
#include <glm.hpp>

struct Light
{
	glm::vec3  m_Position;
	glm::mat4  m_ModelMatrix;
	glm::mat4  m_VPMatrix;
};