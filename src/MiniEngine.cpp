#include "core/Engine.h"
#include "core/Mesh.h"
#include "core/Material.h"
#include "core/Scene.h"

int main(void)
{
	EngineInitParams params{};
	params.screenWidth = 500;
	params.screenHeight = 500;
	params.clearColor = { 0.2f, 0.6f, 0.5f, 1.0f };

	Engine engine{ params };

	auto scene = Scene{ &engine };

	auto meshEntity = scene.createEntity();
	auto cameraEntity = scene.createEntity();

	Mesh::Builder()
		.addBufferData(
			{
				-0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
				0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
				1.0f, -0.0f, 0.0f,  1.0f, 0.0f
			})
		.addIndices(
			{
				0, 1, 2,
				0, 2, 3
			})
		.addBufferAttributes({ 3, 2 })
		.build(&scene, meshEntity);

	auto camera = Camera::Builder()
		.setPosition({ 0,3,-5 })
		.setAspectRatio((float)params.screenWidth / (float)params.screenHeight)
		.setNearFarPlane(0.1f, 1000.0f)
		.setFOV(45)
		.build(&scene, cameraEntity);

	scene.setCameraActive(camera);

	// std funtion to have input callbacks

	/*auto material = Material::Builder()
		.build(&engine, entity);*/

	engine.execute(&scene); //move to separate thread
}
