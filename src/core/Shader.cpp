#include "Shader.h"

Shader::Shader(unsigned int program, const OpenGLDriver* driver) :
	mShaderId(program), mDriver(driver)
{

}

void Shader::enable() const
{
	mDriver->useShaderProgram(mShaderId);
}

void Shader::disable() const
{
	mDriver->useShaderProgram(0);
}
