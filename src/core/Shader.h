#pragma once

#include "../backend/OpenGLDriver.h"

class Shader
{
public:

	Shader() {}
	Shader(unsigned int program, const OpenGLDriver* driver);

	void enable();
	void disable();

private:

	unsigned int mShaderId;
	const OpenGLDriver* mDriver;
};