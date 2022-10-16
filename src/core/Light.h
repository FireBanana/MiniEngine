#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

class Light
{
public:

	inline Light(glm::vec3 position) : m_Position(position)
	{
		m_ModelMatrix = glm::translate(glm::mat4(1.0f), m_Position);

          glm::mat4 lightView = glm::lookAt(m_Position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
          glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
          m_VPMatrix = lightProjection * lightView;
	}

	inline glm::mat4& GetVPMatrix() { return m_VPMatrix; }
    inline glm::vec3& GetPosition() { return m_Position; }
    inline glm::mat4& GetModelMatrix() { return m_ModelMatrix; }

private:
	glm::vec3  m_Position;
	glm::mat4  m_ModelMatrix;
	glm::mat4  m_VPMatrix;
};