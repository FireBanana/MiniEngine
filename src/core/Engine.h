#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>

#include "types/EngineTypes.h"
#include "Entity.h"
#include "IDriver.h"
#include "IPlatform.h"
#include "RenderableComponent.h"
#include "utils/Color.h"
#include "Material.h"
#include "Mesh.h"
#include "GlobalBufferRegistry.h"
#include "ShaderRegistry.h"

namespace MiniEngine
{
	class Engine
	{
	public:

		Engine(MiniEngine::Types::EngineInitParams& params);

		Engine(Engine const&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine const&) = delete;
		Engine& operator=(Engine&&) = delete;

		void execute(Scene* scene);

		Texture loadTexture(const char* path, Texture::TextureType type, bool flipYAxis);

		inline ShaderRegistry* getShaderRegistry() const { return mShaderRegistry.get(); }
		inline GlobalBufferRegistry* getGlobalBufferRegistry() const { return mGlobalBufferRegistry.get(); }
		inline Backend::IDriver* getGraphicsDriver() const { return mGraphicsPlatform.get()->getDriver(); }
		inline MaterialInstance* getDefaultMaterial() { return &mDefaultMaterial; }

		// UI
		void addSlider(const char* name, float* value, float min, float max, std::function<void()> cb = nullptr);
		void addCheckbox(const char* name, bool& flag, std::function<void()> cb = nullptr);

	private:

		std::unique_ptr<GlobalBufferRegistry> mGlobalBufferRegistry;
		std::unique_ptr<Backend::IPlatform> mGraphicsPlatform;
		std::unique_ptr<ShaderRegistry> mShaderRegistry;

		MaterialInstance mDefaultMaterial;
	
		void createDefaultMaterial();
	};
}