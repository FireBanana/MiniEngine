#include "Shader.h"

namespace MiniEngine
{
	Shader::Shader(unsigned int program, const Backend::OpenGLDriver* driver) :
		mShaderId(program), mDriver(driver)
	{

	}

	void Shader::setFloat(const char* name, float value)
	{
		mDriver->setFloat(mShaderId, name, value);
	}

	void Shader::setVec3(const char* name, Vector3 value)
	{
		mDriver->setVec3(mShaderId, name, value);
	}

	void Shader::setMat4(const char* name, Matrix4x4 value)
	{
		mDriver->setMat4(mShaderId, name, value);
	}
}