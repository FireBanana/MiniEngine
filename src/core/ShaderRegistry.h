#pragma once

#include "../backend/OpenGLDriver.h"
#include "Shader.h"

static constexpr char* DEFERRED_VERTEX_PATH = "../shaders/deferred.vert";
static constexpr char* DEFERRED_FRAGMENT_PATH = "../shaders/deferred.frag";

class ShaderRegistry
{
public:

	ShaderRegistry(const OpenGLDriver* driver);

	void loadDeferredShader();
	inline const Shader* getDeferredShader() const noexcept{ return mDefaultShader; }

private:

	const OpenGLDriver* mDriver;
	Shader* mDefaultShader;

	void createShader(unsigned int program, const OpenGLDriver* driver);
};