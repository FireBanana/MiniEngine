#pragma once

#include "../backend/OpenGLDriver.h"

class Shader
{
public:

	Shader() {}
	Shader(unsigned int program, OpenGLDriver* driver);

	void enable();
	void disable();

private:

	unsigned int mShaderId;
	OpenGLDriver* mDriver;
};