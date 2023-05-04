#include "core/Engine.h"
#include "core/Mesh.h"
#include "core/Material.h"
#include "core/Scene.h"
#include "core/Texture.h"

int main(void)
{
	EngineInitParams params{};
	params.screenWidth = 500;
	params.screenHeight = 500;
	params.clearColor = { 0.2f, 0.6f, 0.5f, 1.0f };

	Engine engine{ params };

	auto scene = Scene{ &engine };

	auto meshEntity = scene.createEntity();
	auto meshEntity2 = scene.createEntity();
	auto cameraEntity = scene.createEntity();
	auto cameraEntity2 = scene.createEntity();

	Texture t{ "C:\\Users\\Arthur\\Desktop\\img.png" };

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

	Mesh::Builder()
		.addBufferData(
			{
				-0.0f, -1.0f, -2.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, -2.0f,  0.0f, 1.0f,
				0.0f, 1.0f, -2.0f,   1.0f, 1.0f,
				1.0f, -0.0f, -2.0f,  1.0f, 0.0f
			})
		.addIndices(
			{
				0, 1, 2,
				0, 2, 3
			})
		.addBufferAttributes({ 3, 2 })
		.build(&scene, meshEntity2);

	auto camera = Camera::Builder()
		.setPosition({ 0,3,-5 })
		.setAspectRatio((float)params.screenWidth / (float)params.screenHeight)
		.setNearFarPlane(0.1f, 1000.0f)
		.setFOV(45)
		.build(&scene, cameraEntity);

	auto camera2 = Camera::Builder()
		.setPosition({ 4,4,4 })
		.setAspectRatio((float)params.screenWidth / (float)params.screenHeight)
		.setNearFarPlane(0.1f, 1000.0f)
		.setFOV(45)
		.build(&scene, cameraEntity2);

	scene.setCameraActive(camera2);

	auto material = Material::Builder();

	// std funtion to have input callbacks

	engine.execute(&scene); //move to separate thread
}
