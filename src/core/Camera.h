#pragma once
#include <glm.hpp>
#include <ext/matrix_clip_space.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "interfaces/IMoveable.h"

class Camera : public IMoveable
{
public:

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;

	glm::vec3 m_CameraPosition;

	Camera();	

	virtual void Translate(float x, float y, float z) override;
	virtual void Rotate(float x, float y, float z) override;

};

