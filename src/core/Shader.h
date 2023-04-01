#pragma once

#include "../backend/OpenGLDriver.h"

class Shader
{
public:

	Shader(unsigned int program, const OpenGLDriver* driver);

	Shader() = delete;
	Shader(const Shader&) = delete;
	Shader operator=(const Shader&) = delete;
	Shader(Shader&&) = default;
	Shader& operator=(Shader&&) = default;

	void enable() const;
	void disable() const;

private:

	unsigned int mShaderId;
	const OpenGLDriver* mDriver;
};