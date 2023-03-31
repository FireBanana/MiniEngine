#include "core/Engine.h"
#include "core/Mesh.h"
#include "core/Material.h"

int main(void)
{
	EngineInitParams params;
	params.screenWidth = 800;
	params.screenHeight = 800;
	params.clearColor = { 0.2f, 0.6f, 0.5f, 1.0f };

	Engine engine{ params };

	auto entity = engine.createEntity();

	auto mesh = Mesh::Builder()
		.addVertices(
			{ 
				-0.5f, -0.5f,
				-0.5f, 0.5f,
				0.5f, 0.5f,
				0.5f, -0.5f
			})
		.addIndices(
			{
				0, 1, 2,
				2, 3, 0
			})
		.build(&engine, entity);

	auto material = Material::Builder()
		.setShader(engine.getShaderRegistry()->getDeferredShader())
		.build(&engine, entity);

	engine.execute();
}
