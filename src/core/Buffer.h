#pragma once

#include <glad/glad.h>
#include <vector>
#include <numeric>

template<typename T>
class Buffer
{
public:

	Buffer(unsigned int bufferType);

	void Bind();
	void PushData(std::vector<T>& data, std::vector<int>&& attribSizes);
	void AllocateData(size_t sz);

	inline unsigned int GetId() { return m_Id; }

private:

	void SetAttribPointer(std::vector<T>& data, int size, int stride);

	int m_CurrentIndex;

	unsigned int m_Id;	
	unsigned int m_BufferType;
	unsigned int m_Offset;

};

// ===============
// Implementation
// ===============

template<typename T>
Buffer<T>::Buffer(unsigned int bufferType)
	: m_Offset(0), m_CurrentIndex(-1)
{
	m_BufferType = bufferType;

	if (bufferType == 0)
		glGenVertexArrays(1, &m_Id);
	else
		glGenBuffers(1, &m_Id);
}

template<typename T>
void Buffer<T>::Bind()
{	
	if (m_BufferType == 0)
		glBindVertexArray(m_Id);
	else
		glBindBuffer(m_BufferType, m_Id);
}

template<typename T>
void Buffer<T>::AllocateData(size_t sz)
{
	glBufferData(m_BufferType, sz, nullptr, GL_STATIC_DRAW);
}

template<typename T>
void Buffer<T>::PushData(std::vector<T>& data, std::vector<int>&& attribSizes)
{
	assert(m_BufferType != 0);

	Bind();

	glBufferData(m_BufferType, sizeof(T) * data.size(), &data[0], GL_STATIC_DRAW);

	int stride = std::accumulate(attribSizes.begin(), attribSizes.end(), 0);

	for (auto size : attribSizes)
	{
		SetAttribPointer(data, size, stride);		
		m_Offset += size;
	}
}

template<typename T>
void Buffer<T>::SetAttribPointer(std::vector<T>& data, int size, int stride)
{	
	glVertexAttribPointer(++m_CurrentIndex, size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(m_Offset * sizeof(float)));
	glEnableVertexAttribArray(m_CurrentIndex);
}