#pragma once
#include "Shader.h"
#include "Buffer.h"
#include "Camera.h"
#include "interfaces/IDrawable.h"
#include "interfaces/IMoveable.h"

#include <vector>
#include <glm.hpp>

class Object : public IMoveable, public IDrawable
{
public:

	Object(Shader s, std::vector<float> vertices, int triangleCount, std::vector<int>&& attribSizes);

	void Draw(Camera camera) override;
	virtual void Translate(float x, float y, float z) override;
	virtual void Rotate(float x, float y, float z) override;

private:

	Shader			   m_Shader;
	std::vector<float> m_Vertices;
	int				   m_TriangleCount;
	Buffer<float>	   m_VertexBuffer;
	Buffer<float>	   m_VertexArray;
};

