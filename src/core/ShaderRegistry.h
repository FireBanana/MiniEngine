#pragma once

#include <vector>
#include "../backend/OpenGLDriver.h"
#include "Shader.h"

static constexpr char* DEFERRED_VERTEX_PATH = "../shaders/deferred.vert";
static constexpr char* DEFERRED_FRAGMENT_PATH = "../shaders/deferred.frag";
static constexpr char* LIGHTING_VERTEX_PATH = "../shaders/pbr.vert";
static constexpr char* LIGHTING_FRAGMENT_PATH = "../shaders/pbr.frag";

class ShaderRegistry
{
public:

	ShaderRegistry(const OpenGLDriver* driver);

	void loadDeferredShader();
	void loadPbrShader();

	inline Shader* getDeferredShader() noexcept { return &mShaderTable[0]; }
	inline Shader* getPbrShader() noexcept { return &mShaderTable[1]; }

	void setActiveUniformBuffer(const char* blockName, size_t dataSize, void* data);

	void enable(Shader* shader);

private:

	std::vector<Shader> mShaderTable;
	const OpenGLDriver* mDriver;
	Shader* mActiveShader;
	
	void createShader(unsigned int program, const OpenGLDriver* driver);
};