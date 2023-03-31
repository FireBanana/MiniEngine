#pragma once

#include "types/EngineTypes.h"
#include <GLFW/glfw3.h>
#include "../backend/OpenGLPlatform.h"
#include "utils/Color.h"
#include "Entity.h"

class Mesh;
class Material;
class OpenGLPlatform;


class Engine
{
public:

	Engine(const EngineInitParams& params);

	Engine(Engine const&) = delete;
	Engine(Engine &&) = delete;
	Engine& operator=(Engine const&) = delete;
	Engine& operator=(Engine &&) = delete;

	void execute();

	Entity* createEntity();

	inline const ShaderRegistry* getShaderRegistry() const { return mShaderRegistry.get(); }

private:

	std::unique_ptr<OpenGLPlatform> mGlPlatform;	
	std::unique_ptr<ShaderRegistry> mShaderRegistry;
};