#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include "types/EngineTypes.h"
#include "Entity.h"
#include "../backend/OpenGLPlatform.h"
#include "../components/MeshComponent.h"
#include "utils/Color.h"
#include "Mesh.h"
#include "Material.h"

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

	void createMesh(const Mesh::BuilderResults& builderResults, Entity* entity);
	void createMaterial(const Material::Builder* builder, Entity* entity);

	inline const ShaderRegistry* getShaderRegistry() const { return mShaderRegistry.get(); }
	inline const std::vector<MeshComponent>* getMeshComponentDatabase() const { return &mMeshComponentDatabase; }

private:

	std::unique_ptr<OpenGLPlatform> mGlPlatform;	
	std::unique_ptr<ShaderRegistry> mShaderRegistry;

	std::vector<Entity> mEntityDatabase;

	std::vector<MeshComponent> mMeshComponentDatabase;
};