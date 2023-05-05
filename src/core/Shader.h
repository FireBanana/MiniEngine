#pragma once

#include "../backend/OpenGLDriver.h"

class Shader
{
public:

	Shader(unsigned int program, const OpenGLDriver* driver);

	Shader() {};
	Shader(const Shader&) = default;
	Shader& operator=(const Shader&) = default;
	Shader(Shader&&) = default;
	Shader& operator=(Shader&&) = default;

	inline const unsigned int getShaderProgram() const { return mShaderId; }

	//move these to separate uniformbuffer?}
	inline unsigned int* getUniformBlockBufferPoint() { return &mUniformBufferIndex; }

private:

	unsigned int mShaderId;
	const OpenGLDriver* mDriver;

	unsigned int mUniformBufferIndex;
};