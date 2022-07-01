#pragma once
#include "Buffer.h"
#include "Shader.h"

class UniformGLData
{
public:

	struct BufferStructure
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 lightvp;
		glm::vec3 lightPos;
	};

	static UniformGLData& Get()
	{
		static UniformGLData u;
		return u;
	}

	void RegisterShader(Shader& shader, std::string name)
	{
		m_MatrixUniformBuffer.Bind();
		unsigned int id = glGetUniformBlockIndex(shader.GetShaderId(), name.c_str());
		glUniformBlockBinding(shader.GetShaderId(), id, 1);
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_MatrixUniformBuffer.GetId(), 0, 512);		
	}

	void UpdateBufferData(unsigned int offset, float size, void* data)
	{
		m_MatrixUniformBuffer.Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}

private: 

	Buffer<float> m_MatrixUniformBuffer;

	UniformGLData() : m_MatrixUniformBuffer(GL_UNIFORM_BUFFER)
	{
		m_MatrixUniformBuffer.Bind();
		m_MatrixUniformBuffer.AllocateData(512);
	}
};