#include "Object.h" 
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <gtc/type_ptr.hpp>


Object::Object(Shader s, std::vector<Constants::Vertex>& vertices, int triangleCount, std::vector<int>&& attribSizes)
	: m_Shader(s), m_Vertices(vertices), m_TriangleCount(triangleCount), m_VertexBuffer(GL_ARRAY_BUFFER),
	m_VertexArray(0)
{	
	m_ModelMatrix = glm::mat4(1.0f);

	m_VertexArray.Bind();
	m_VertexBuffer.PushData(vertices, std::move(attribSizes));
}

void Object::Draw(Camera camera)
{
	m_VertexArray.Bind();
	//m_Shader.Use();
	m_Shader.SetUniform_m("model", m_ModelMatrix);

	glDrawArrays(GL_TRIANGLES, 0, m_TriangleCount);
}

void Object::Draw(Camera camera, Shader& shader)
{
	m_VertexArray.Bind();
	m_Shader.SetUniform_m("model", m_ModelMatrix, shader);

	glDrawArrays(GL_TRIANGLES, 0, m_TriangleCount);
}

void Object::Translate(float x, float y, float z)
{
	m_ModelMatrix = glm::translate(m_ModelMatrix, glm::vec3(x, y, z));
}

void Object::Rotate(float x, float y, float z)
{
	m_ModelMatrix = glm::rotate(m_ModelMatrix, x, glm::vec3(1.0f, 0.0f, 0.0f));
	m_ModelMatrix = glm::rotate(m_ModelMatrix, y, glm::vec3(0.0f, 1.0f, 0.0f));
	m_ModelMatrix = glm::rotate(m_ModelMatrix, z, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Object::Scale(float x, float y, float z)
{
	m_ModelMatrix = glm::scale(m_ModelMatrix, glm::vec3(x,y,z));
}

void Object::AddTexture(const char* path, int textureUnit)
{
	int width, height, channels;

	auto img =
		stbi_load(path, &width, &height, &channels, 0);

	if (img == nullptr) std::cout << "Error loading image";

	glActiveTexture(textureUnit);
	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(img);
}

