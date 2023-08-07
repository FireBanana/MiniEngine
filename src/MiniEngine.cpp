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
	params.screenWidth = 1920;
	params.screenHeight = 1080;
	params.clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	MiniEngine::Engine engine{ params };

	auto scene = MiniEngine::Scene{ &engine };

	auto meshEntity = scene.createEntity();
	auto skyBoxEntity = scene.createEntity();
	auto cameraEntity2 = scene.createEntity();
	auto mainLightEntity = scene.createEntity();

	MiniEngine::Texture texture = engine.loadTexture("C:\\Users\\Arthur\\Desktop\\red.png", MiniEngine::Texture::TextureType::Default, false);
	MiniEngine::Texture normal = engine.loadTexture("C:\\Users\\Arthur\\Desktop\\normal.png", MiniEngine::Texture::TextureType::Default, false);
	MiniEngine::Texture texture2 = engine.loadTexture("C:\\Users\\Arthur\\Desktop\\hdri2.hdr", MiniEngine::Texture::TextureType::CubeMap, true);

	auto material = MiniEngine::Material::Creator()
		.addTexture(MiniEngine::Material::TextureType::Diffuse, texture)
		.addTexture(MiniEngine::Material::TextureType::Normal, normal)
		.addShader(engine.getShaderRegistry()->getDeferredShader())
		.addMaterialProperty(MiniEngine::Material::PropertyType::Roughness, 0.15f)
		.addMaterialProperty(MiniEngine::Material::PropertyType::Metallic, 1.f)
		.create();

	auto mesh = MiniEngine::Renderable::Builder()
		.addModel("C:\\Users\\Arthur\\Desktop\\sphere.glb")
		.addMaterialInstance(&material)
		.build(&scene, meshEntity);

	auto skyBox = MiniEngine::Skybox::Builder()
		.setTexture(texture2)
		.build(&scene, skyBoxEntity);

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
		.setPosition({(float)sqrt(5), 2, (float)sqrt(5) })
		.setAspectRatio((float)params.screenWidth / (float)params.screenHeight)
		.setNearFarPlane(0.1f, 1000.0f)
		.setFOV(45)
		.build(&scene, cameraEntity2);

	auto light = MiniEngine::Light::Builder()
		.setPosition({ 0,0,0 })
		.setIntensity(1.0f)
		.build(&scene, mainLightEntity);

	// todo, hides when x/z 0. Need to set forward vector etc.
	scene.setCameraActive(camera);
	
	//scene.addLight(light);
	scene.setSkyboxActive(&skyBox);
	//scene.addLight(light);

	engine.addSlider("Skybox", &(skyBox.rotation), 0.0f, 3.142f * 2);
	engine.addSlider("Model", &(mesh->rotation.y), 0.0f, 360);

	engine.addSlider("Light-X", &(light->position.x), -1, 1, [&]() { scene.addLight(light); });
	engine.addSlider("Light-Y", &(light->position.y), -1, 1, [&]() { scene.addLight(light); });
	engine.addSlider("Light-Z", &(light->position.z), -1, 1, [&]() { scene.addLight(light); });
	engine.addSlider("Light-Intensity", &(light->intensity), 0, 10, [&]() { scene.addLight(light); });

	float cameraRot = 0;
	engine.addSlider("Camera-X", &(cameraRot), 0, 3.142f * 2, [&]() 
		{ 
			camera->position.x = 3 * cos(cameraRot);
			camera->position.z = 3 * sin(cameraRot);
			scene.setCameraActive(camera); 
			skyBox.rotation = cameraRot;
		});

	float r = material.materialProperties[(int)MiniEngine::Material::PropertyType::Roughness];
	float g = material.materialProperties[(int)MiniEngine::Material::PropertyType::Metallic];
	engine.addSlider("Roughness", &r, 0, 1, [&]() { material.materialProperties.set((int)MiniEngine::Material::PropertyType::Roughness, r); });
	engine.addSlider("Metallic", &g, 0, 1, [&]() { material.materialProperties.set((int)MiniEngine::Material::PropertyType::Metallic, g); });

	engine.execute(&scene); //move to separate thread
}
