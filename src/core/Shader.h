#pragma once

#include "../backend/OpenGLDriver.h"

class Shader
{
public:

	Shader(unsigned int program, const OpenGLDriver* driver);

	Shader() = delete;
	Shader(const Shader&) = delete;
	Shader operator=(const Shader&) = delete;
	Shader(const Shader&&) = delete;
	Shader operator=(const Shader&&) = delete;

	void enable();
	void disable();

private:

	unsigned int mShaderId;
	const OpenGLDriver* mDriver;
};