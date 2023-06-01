#include "core/Engine.h"
#include "core/Renderable.h"
#include "core/Material.h"
#include "core/Scene.h"
#include "core/Texture.h"

#include <thread>
#include <iostream>
#include <functional>
#include <glm/glm.hpp>

int main(void)
{
	EngineInitParams params{};
	params.screenWidth = 1200;
	params.screenHeight = 800;
	params.clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	Engine engine{ params };

	auto scene = Scene{ &engine };

	auto meshEntity = scene.createEntity();
	auto meshEntity2 = scene.createEntity();
	auto cameraEntity = scene.createEntity();
	auto cameraEntity2 = scene.createEntity();

	Texture texture = engine.loadTexture("C:\\Users\\Arthur\\Desktop\\1.png");
	Material material{ engine.getShaderRegistry()->getDeferredShader() };
	material.addTexture(0, texture);

	auto mesh = Renderable::Builder()
		.addModel("C:\\Users\\Arthur\\Desktop\\din.glb")
		.addMaterial(&material)
		.build(&scene, meshEntity);

	//Renderable::Builder()
	//	.addBufferData(
	//		{
	//			-0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	//			-1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	//			0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
	//			1.0f, -0.0f, 0.0f,  1.0f, 0.0f
	//		})
	//	.addIndices(
	//		{
	//			0, 1, 2,
	//			0, 2, 3
	//		})
	//	.addBufferAttributes({ 3, 2 })
	//	.addMaterial(&material)
	//	.build(&scene, meshEntity);

	//Renderable::Builder()
	//	.addBufferData(
	//		{
	//			-0.0f, -1.0f, -2.0f, 0.0f, 0.0f,
	//			-1.0f, 0.0f, -2.0f,  0.0f, 1.0f,
	//			0.0f, 1.0f, -2.0f,   1.0f, 1.0f,
	//			1.0f, -0.0f, -2.0f,  1.0f, 0.0f
	//		})
	//	.addIndices(
	//		{
	//			0, 1, 2,
	//			0, 2, 3
	//		})
	//	.addBufferAttributes({ 3, 2 })
	//	.build(&scene, meshEntity2);

	auto camera = Camera::Builder()
		.setPosition({2,2,2 })
		.setAspectRatio((float)params.screenWidth / (float)params.screenHeight)
		.setNearFarPlane(0.1f, 1000.0f)
		.setFOV(45)
		.build(&scene, cameraEntity2);

	scene.setCameraActive(camera);

	// std funtion to have input callbacks

	//mesh->worldPosition.y = -20;

	auto t = std::thread([&]()
		{	
			float t = 0.0f;
			float d = 0.0f;

			while (1)
			{
				d = glfwGetTime() - t;
				t = glfwGetTime();
				mesh->rotation.y += 100 * d;

			}
		});

	engine.execute(&scene); //move to separate thread
}
