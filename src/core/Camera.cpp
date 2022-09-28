#include "Camera.h"

Camera::Camera()
	: m_CameraPosition({3.0f, 3.0f, -3.0f})
{
	m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
	m_ViewMatrix = glm::lookAt(m_CameraPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Translate(float x, float y, float z)
{
	m_CameraPosition = glm::vec3(x, y, z);
}

void Camera::Rotate(float x, float y, float z)
{

}

void Camera::Scale(float x, float y, float z)
{
}
