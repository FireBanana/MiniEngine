#pragma once

class Mesh
{
public:



private:

	// buffer data (can be multiple)
	size_t mByteLength;
	size_t mVertexBufferByteOffset;
	unsigned char* mVertexBufferStart;

	// attribute data
	int mVertexAttribIndex;
	int mVertexAttribSize;
	int mVertexAttribType;
	int mVertexStride;
	size_t mVertexBufferOffset;
	bool mVertexAttribNormalized;

	// texture data (not here)
	int mTextureWidth;
	int mTextureHeight;
	int mTextureFormat;
	int mTextureType;
	unsigned char* mTextureStart;
};