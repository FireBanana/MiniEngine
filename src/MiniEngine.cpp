#include "core/Engine.h"
#include "core/Mesh.h"
#include "core/Material.h"
#include "core/RenderPass.h"
#include "core/Scene.h"

int main(void)
{
	EngineInitParams params{};
	params.screenWidth = 500;
	params.screenHeight = 500;
	params.clearColor = { 0.2f, 0.6f, 0.5f, 1.0f };

	Engine engine{ params };

	auto scene = Scene{};
	auto entity = scene.createEntity();

	Mesh::Builder()
		.addBufferData(
			{ 
				-0.0f, -1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f,  0.0f, 1.0f,
				0.0f, 1.0f,   1.0f, 1.0f,
				1.0f, -0.0f,  1.0f, 0.0f
			})
		.addIndices(
			{
				0, 1, 2,
				0, 2, 3
			})
		.addBufferAttributes({2, 2})
		.build(&scene, entity);

	auto material = Material::Builder()
		.build(&engine, entity);

	engine.execute(&scene); //move to separate thread
}
