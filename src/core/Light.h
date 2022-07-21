#pragma once
#include <glm.hpp>

struct Light
{
	glm::vec3  Position;
	glm::mat4  ModelMatrix;
	glm::mat4  VPMatrix;
};