#pragma once

#include <glm.hpp>

class IMoveable
{
public:

	virtual void Translate(float x, float y, float z) = 0;
	virtual void Rotate(float x, float y, float z) = 0;
	virtual void Scale(float x, float y, float z) = 0;

protected:

	//Should not be in interface
	glm::mat4 m_ModelMatrix;

};