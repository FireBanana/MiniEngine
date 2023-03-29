#pragma once
#include "../core/types/EngineTypes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class OpenGLDriver
{
public:

	enum class ShaderType : GLenum
	{
		VERTEX  = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER
	};

	void setupGlWindowParams(const EngineInitParams& params);
	void setupFrameBuffer();
	void setupDebugInfo();

	void draw();
	void finalBlit();

	void beginRenderpass();
	void endRenderpass();

	unsigned int loadShader(const char* path, ShaderType type) const;
	unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const;

	void useShaderProgram(unsigned int program) const;

private:

	GLuint mMainFrameBuffer;
	
	uint16_t mWidth;
	uint16_t mHeight;
};