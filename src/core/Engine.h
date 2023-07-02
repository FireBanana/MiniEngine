#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

#include "types/EngineTypes.h"
#include "Entity.h"
#include "OpenGLPlatform.h"
#include "RenderableComponent.h"
#include "utils/Color.h"
#include "Material.h"
#include "Mesh.h"
#include "GlobalBufferRegistry.h"

namespace MiniEngine
{
	namespace
	{
		class OpenGLPlatform;
	}

	class Engine
	{
	public:

		Engine(const EngineInitParams& params);

		Engine(Engine const&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine const&) = delete;
		Engine& operator=(Engine&&) = delete;

		void execute(Scene* scene);

		Texture loadTexture(const char* path, Texture::TextureType type);

		inline ShaderRegistry* getShaderRegistry() const { return mShaderRegistry.get(); }
		inline GlobalBufferRegistry* getGlobalBufferRegistry() const { return mGlobalBufferRegistry.get(); }
		inline Backend::OpenGLDriver* getOpenGlDriver() const { return mGlPlatform.get()->getDriver(); }

		// UI
		void addSlider(float* value, float min, float max);

	private:

		std::unique_ptr<GlobalBufferRegistry> mGlobalBufferRegistry;
		std::unique_ptr<Backend::OpenGLPlatform> mGlPlatform;
		std::unique_ptr<ShaderRegistry> mShaderRegistry;
	};
}