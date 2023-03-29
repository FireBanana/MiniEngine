#include "Shader.h"

Shader::Shader(unsigned int program, const OpenGLDriver* driver) :
	mShaderId(program), mDriver(driver)
{

}

void Shader::enable()
{
	mDriver->useShaderProgram(mShaderId);
}

void Shader::disable()
{
	mDriver->useShaderProgram(0);
}
