#pragma once
#include "Shader.h"
#include "Buffer.h"
#include "Camera.h"
#include "interfaces/IDrawable.h"
#include "interfaces/IMoveable.h"
#include "utils/Constants.h"

#include <vector>
#include <glm.hpp>

class Object : public IMoveable, public IDrawable
{
public:

	Object(Shader s, std::vector<Constants::Vertex>& vertices, int triangleCount, std::vector<int>&& attribSizes);

	void Draw(Camera camera) override;
	void Draw(Camera camera, Shader& shader); //Add to drawable
	virtual void Translate(float x, float y, float z) override;
	virtual void Rotate(float x, float y, float z) override;
	virtual void Scale(float x, float y, float z) override;

	void AddTexture(const char* path, int textureUnit);

private:

	Shader						   m_Shader;
	std::vector<Constants::Vertex> m_Vertices;
	int							   m_TriangleCount;
	Buffer<Constants::Vertex>	   m_VertexBuffer;
	Buffer<Constants::Vertex>	   m_VertexArray;

	unsigned int				   m_Texture;
};

