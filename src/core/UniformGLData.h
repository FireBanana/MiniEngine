#pragma once
#include "Buffer.h"
#include "Shader.h"

class UniformGLData
{
public:

	static UniformGLData& Get()
	{
		static UniformGLData u;
		return u;
	}

	void RegisterShader(Shader& shader, std::string name)
	{
		unsigned int id = glGetUniformBlockIndex(shader.GetShaderId(), name.c_str());
		glUniformBlockBinding(shader.GetShaderId(), id, 3);
		glBindBufferRange(GL_UNIFORM_BUFFER, 3, m_MatrixUniformBuffer.GetId(), 0, 512);
	}

	void UpdateBufferData(unsigned int size, void* data)
	{
		m_MatrixUniformBuffer.Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, size, data);

		m_Offset += size;
	}

private: 

	Buffer<float> m_MatrixUniformBuffer;
	int			  m_Offset;

	UniformGLData() : m_Offset(0), m_MatrixUniformBuffer(GL_UNIFORM_BUFFER)
	{
		// Layout (bytes)
		// mat4 projection
		// mat4 view 
		// vec3 lightPos
		// mat4 lightvp

		m_MatrixUniformBuffer.Bind();
		m_MatrixUniformBuffer.AllocateData(512);
	}
};