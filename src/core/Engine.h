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

class OpenGLPlatform;

class Engine
{
public:

	Engine(const EngineInitParams& params);

	Engine(Engine const&) = delete;
	Engine(Engine &&) = delete;
	Engine& operator=(Engine const&) = delete;
	Engine& operator=(Engine &&) = delete;

	void execute(Scene* scene);

	Texture loadTexture(const char* path);
	RenderableComponent loadMesh(const char* path);

	inline ShaderRegistry* getShaderRegistry() const { return mShaderRegistry.get(); }
	inline OpenGLDriver* getOpenGlDriver() const { return mGlPlatform.get()->getDriver(); }

private:

	std::unique_ptr<OpenGLPlatform> mGlPlatform;	
	std::unique_ptr<ShaderRegistry> mShaderRegistry;

	void makeRenderable_internal();
};