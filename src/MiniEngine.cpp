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
	MiniEngine::Engine engine{ params };

	auto scene = MiniEngine::Scene{ &engine };

	auto meshEntity = scene.createEntity();
	auto meshEntity2 = scene.createEntity();
	auto cameraEntity2 = scene.createEntity();
	auto mainLightEntity = scene.createEntity();

	MiniEngine::Texture texture = engine.loadTexture("C:\\Users\\Owais\\Desktop\\img.png");

	auto material = MiniEngine::Material::Creator()
		.addTexture(MiniEngine::Material::TextureType::Diffuse, texture)
		.addShader(engine.getShaderRegistry()->getDeferredShader())
		.addMaterialProperty(MiniEngine::Material::PropertyType::Roughness, 0.025f)
		.create();

	auto mesh = MiniEngine::Renderable::Builder()
		.addModel("C:\\Users\\Owais\\Desktop\\dino2.glb")
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

	auto camera = MiniEngine::Camera::Builder()
		.setPosition({60,60,60 })
		.setAspectRatio((float)params.screenWidth / (float)params.screenHeight)
		.setNearFarPlane(0.1f, 1000.0f)
		.setFOV(45)
		.build(&scene, cameraEntity2);

	auto light = MiniEngine::Light::Builder()
		.setPosition({ 4.0f,3.0f,-1.0f })
		.setIntensity(1.0f)
		.build(&scene, mainLightEntity);

	scene.setCameraActive(camera);
	
	scene.addLight(light);
	//scene.removeLight();

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
