#pragma once
//#include "../core/VertexBuffer.h"
//#include "../core/IndexBuffer.h"
#include "../core/types/EngineTypes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class OpenGLDriver
{
public:

	void setupGlWindowParams(const EngineInitParams& params);
	void setupFrameBuffer();
	void setupDebugInfo();

	void draw();
	void finalBlit();

	void beginRenderpass();
	void endRenderpass();

private:

	GLuint mMainFrameBuffer;
	
	uint16_t mWidth;
	uint16_t mHeight;
};