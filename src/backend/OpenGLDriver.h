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

	unsigned int loadShader(const char* path, ShaderType type);
	unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);

	void useShaderProgram(unsigned int program);

private:

	GLuint mMainFrameBuffer;
	
	uint16_t mWidth;
	uint16_t mHeight;
};