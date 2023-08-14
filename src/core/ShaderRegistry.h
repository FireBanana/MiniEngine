#pragma once

#include <vector>
#include "OpenGLDriver.h"
#include "Shader.h"

#define RESOLVE_PATH(path) DIR##path

static constexpr char* DEFERRED_VERTEX_PATH		   = RESOLVE_PATH("/shaders/deferred.vert");
static constexpr char* DEFERRED_FRAGMENT_PATH	   = RESOLVE_PATH("/shaders/deferred.frag");
static constexpr char* LIGHTING_VERTEX_PATH		   = RESOLVE_PATH("/shaders/pbr.vert");
static constexpr char* LIGHTING_FRAGMENT_PATH	   = RESOLVE_PATH("/shaders/pbr.frag");
static constexpr char* SKYBOX_VERTEX_PATH		   = RESOLVE_PATH("/shaders/sky.vert");
static constexpr char* SKYBOX_FRAGMENT_PATH		   = RESOLVE_PATH("/shaders/sky.frag");
static constexpr char* SKYBOX_RENDER_VERTEX_PATH   = RESOLVE_PATH("/shaders/sky_render.vert");
static constexpr char* SKYBOX_RENDER_FRAGMENT_PATH = RESOLVE_PATH("/shaders/sky_render.frag");
static constexpr char* ENV_CONVOLUTION_PATH		   = RESOLVE_PATH("/shaders/env_convolution.frag");
static constexpr char* ENV_PREFILTER_PATH		   = RESOLVE_PATH("/shaders/prefilter.frag");
static constexpr char* ENV_PRECOMPUTE_BRDF_PATH	   = RESOLVE_PATH("/shaders/precompbrdf.frag");

namespace MiniEngine
{
	class ShaderRegistry
	{
	public:

		ShaderRegistry(const Backend::OpenGLDriver* driver);

		void loadDeferredShader();
		void loadPbrShader();
		void loadSkyboxShader();
		void loadSkyboxRenderShader();
		void loadSkyboxConvoluter();
		void loadEnvPrefilterShader();
		void loadEnvPreComputeBrdfShader();

		inline Shader* getDeferredShader()			noexcept { return &mShaderTable[0]; }
		inline Shader* getPbrShader()				noexcept { return &mShaderTable[1]; }
		inline Shader* getSkyboxShader()			noexcept { return &mShaderTable[2]; }
		inline Shader* getSkyboxRenderShader()		noexcept { return &mShaderTable[3]; }
		inline Shader* getSkyboxConvoluter()		noexcept { return &mShaderTable[4]; }
		inline Shader* getEnvPrefilterShader()		noexcept { return &mShaderTable[5]; }
		inline Shader* getEnvPreComputeBrdfShader() noexcept { return &mShaderTable[6]; }

		void enable(Shader* shader);
		void bindGlobalBufferToAll(const char* name, unsigned int bindIndex);

		inline const Shader* getActiveShader() const { return mActiveShader; }

	private:

		std::vector<Shader>			 mShaderTable;
		const Backend::OpenGLDriver* mDriver;
		Shader*						 mActiveShader;

		void createShader(unsigned int program, const Backend::OpenGLDriver* driver);
	};
}