#pragma once

#include "../backend/OpenGLDriver.h"
#include "Shader.h"

static constexpr char* DEFERRED_VERTEX_PATH = "../shaders/deferred.vert";
static constexpr char* DEFERRED_FRAGMENT_PATH = "../shaders/deferred.frag";

class ShaderRegistry
{
public:

	ShaderRegistry() {};
	ShaderRegistry(OpenGLDriver* driver);

	void loadDeferredShader();
	inline Shader getDeferredShader() const noexcept{ return mDefaultShader; }

private:

	OpenGLDriver* mDriver;

	Shader mDefaultShader;

};