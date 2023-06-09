#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include "types/EngineTypes.h"
#include "Entity.h"
#include "../backend/OpenGLPlatform.h"
#include "../components/RenderableComponent.h"
#include "utils/Color.h"
#include "Material.h"
#include "Mesh.h"
#include "GlobalBufferRegistry.h"

namespace MiniEngine
{
	class Backend::OpenGLPlatform;

	class Engine
	{
	public:

		Engine(const EngineInitParams& params);

		Engine(Engine const&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine const&) = delete;
		Engine& operator=(Engine&&) = delete;

		void execute(Scene* scene);

		Texture loadTexture(const char* path);

		inline ShaderRegistry* getShaderRegistry() const { return mShaderRegistry.get(); }
		inline GlobalBufferRegistry* getGlobalBufferRegistry() const { return mGlobalBufferRegistry.get(); }
		inline Backend::OpenGLDriver* getOpenGlDriver() const { return mGlPlatform.get()->getDriver(); }

	private:

		std::unique_ptr<GlobalBufferRegistry> mGlobalBufferRegistry;
		std::unique_ptr<Backend::OpenGLPlatform> mGlPlatform;
		std::unique_ptr<ShaderRegistry> mShaderRegistry;
	};
}