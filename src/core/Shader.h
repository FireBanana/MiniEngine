#pragma once

#include "IDriver.h"
#include "EngineTypes.h"

namespace MiniEngine
{
	class Shader
	{
	public:

		Shader(unsigned int program, const Backend::IDriver* driver);

		Shader() {};
		Shader(const Shader&) = default;
		Shader& operator=(const Shader&) = default;
		Shader(Shader&&) = default;
		Shader& operator=(Shader&&) = default;

		inline const unsigned int getShaderProgram() const { return mShaderId; }

		//move these to separate uniformbuffer?}
		inline unsigned int* getUniformBlockBufferPoint() { return &mUniformBufferIndex; }

		void setFloat(const char* name, float value);
		void setVec3(const char* name, Vector3 value);
		void setMat4(const char* name, Matrix4x4 value);

	private:

		unsigned int				 mShaderId;
		const Backend::IDriver*		 mDriver;

		unsigned int				 mUniformBufferIndex;
	};
}