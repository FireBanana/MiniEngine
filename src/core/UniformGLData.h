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

	struct UniformBufferTemplate
	{
		unsigned int offset;
		float		 size;
		void*		 data;
	};

	UniformGLData(Shader& shader, const char* uniformName, std::initializer_list<UniformBufferTemplate> data)
		: m_MatrixUniformBuffer(GL_UNIFORM_BUFFER)
	{
		m_MatrixUniformBuffer.Bind();
		m_MatrixUniformBuffer.AllocateData(512);
		
		RegisterShader(shader, uniformName);
	
		for (auto ptr : data)
		{
			UpdateBufferData(ptr.offset, ptr.size, ptr.data);
		}
	}

	void RegisterShader(Shader& shader, const char* name)
	{
		m_MatrixUniformBuffer.Bind();
		unsigned int id = glGetUniformBlockIndex(shader.GetShaderId(), name);
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
};