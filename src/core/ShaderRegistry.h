#pragma once

#include <vector>
#include "../backend/OpenGLDriver.h"
#include "Shader.h"

static constexpr char* DEFERRED_VERTEX_PATH = "../shaders/deferred.vert";
static constexpr char* DEFERRED_FRAGMENT_PATH = "../shaders/deferred.frag";

class ShaderRegistry
{
public:

	ShaderRegistry(const OpenGLDriver* driver);

	void loadDeferredShader();
	inline const Shader* getDeferredShader() const noexcept { return &mShaderTable[0]; }

private:

	std::vector<Shader> mShaderTable;

	const OpenGLDriver* mDriver;
	void createShader(unsigned int program, const OpenGLDriver* driver);
};