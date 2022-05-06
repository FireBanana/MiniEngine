#pragma once

#include <glm.hpp>

class IMoveable abstract
{
public:

	virtual void Translate(float x, float y, float z) = 0;
	virtual void Rotate(float x, float y, float z) = 0;

protected:

	glm::mat4 m_ModelMatrix;

};