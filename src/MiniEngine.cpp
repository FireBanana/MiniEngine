#include "core/Engine.h"
#include "core/Mesh.h"
#include "core/Material.h"
#include "core/RenderPass.h"
#include "core/Scene.h"

int main(void)
{
	EngineInitParams params;
	params.screenWidth = 800;
	params.screenHeight = 500;
	params.clearColor = { 0.2f, 0.6f, 0.5f, 1.0f };

	Engine engine{ params };

	auto entity = engine.createEntity();

	auto scene = Scene{};

	Mesh::Builder()
		.addVertices(
			{ 
				-1.5f, -1.5f,
				-0.5f, 0.5f,
				0.5f, 0.5f,
				0.5f, -0.5f
			})
		.addIndices(
			{
				0, 1, 2,
				0, 2, 3
			})
		.build(&engine, entity);

	auto material = Material::Builder()
		.setShader(engine.getShaderRegistry()->getDeferredShader())
		.build(&engine, entity);

	scene.addEntity(entity);

	engine.execute(); //move to separate thread
}
