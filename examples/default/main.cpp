#include "Engine.h"
#include "Renderable.h"
#include "Material.h"
#include "Scene.h"
#include "Texture.h"

#include <thread>
#include <iostream>
#include <functional>

#include <vulkan/vulkan.h>
#include "GlslCompiler.h"

#define RESOLVE_PATH(path) DIR##path

int main(void)
{
	MiniEngine::Types::EngineInitParams params{};
	params.screenWidth = 640;
	params.screenHeight = 480;
	params.clearColor = { 0.1f, 0.15f, 0.2f, 0.0f };
	params.enablePostProcess = false;
	MiniEngine::Engine engine{ params };

	auto scene = MiniEngine::Scene{ &engine };

	//auto meshEntity = scene.createEntity();
	//auto skyBoxEntity = scene.createEntity();
	//auto cameraEntity2 = scene.createEntity();
	//auto mainLightEntity = scene.createEntity();

	//MiniEngine::Texture texture = engine.loadTexture(RESOLVE_PATH("/assets/color.jpg"), MiniEngine::Texture::TextureType::Default, false);
	//MiniEngine::Texture normal = engine.loadTexture(RESOLVE_PATH("/assets/normal.jpg"), MiniEngine::Texture::TextureType::Default, false);
	//MiniEngine::Texture roughness = engine.loadTexture(RESOLVE_PATH("/assets/roughness.jpg"), MiniEngine::Texture::TextureType::Default, false);
	//MiniEngine::Texture texture2 = engine.loadTexture(RESOLVE_PATH("/assets/hdri.hdr"), MiniEngine::Texture::TextureType::CubeMap, true);

	//auto material = MiniEngine::Material::Creator()
	//	.addTexture(MiniEngine::Types::TextureType::Diffuse, texture)
	//	.addTexture(MiniEngine::Types::TextureType::Normal, normal)
	//	.addTexture(MiniEngine::Types::TextureType::Roughness, roughness)
	//	.addShader(engine.getShaderRegistry()->getDeferredShader())
	//	.addMaterialProperty(MiniEngine::Material::PropertyType::Roughness, 0.15f)
	//	.addMaterialProperty(MiniEngine::Material::PropertyType::Metallic, 0.f)
	//	.create();

	//auto mesh = MiniEngine::Renderable::Builder()
	//	.addModel(RESOLVE_PATH("/assets/sphere.glb"))
	//	.addMaterialInstance(&material)
	//	.build(&scene, meshEntity);

	//auto skyBox = MiniEngine::Skybox::Builder()
	//	.setTexture(texture2)
	//	.build(&scene, skyBoxEntity);

	////auto mesh = MiniEngine::Renderable::Builder()
	////	.addBufferData(
	////		{
	////			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	////			-1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
	////			1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
	////			1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f
	////		})
	////	.addIndices(
	////		{
	////			0, 1, 2,
	////			0, 2, 3
	////		})
	////	.addBufferAttributes({ 3, 2, 3 })
	////	.addMaterialInstance(&material)
	////	.build(&scene, meshEntity);

	////auto mesh = MiniEngine::Renderable::Builder()
	////	.addBufferData(
	////		{
	////			-0.0f, -1.0f, -2.0f, 0.0f, 0.0f,
	////			-1.0f, 0.0f, -2.0f,  0.0f, 1.0f,
	////			0.0f, 1.0f, -2.0f,   1.0f, 1.0f,
	////			1.0f, -0.0f, -2.0f,  1.0f, 0.0f
	////		})
	////	.addIndices(
	////		{
	////			0, 1, 2,
	////			0, 2, 3
	////		})
	////	.addBufferAttributes({ 3, 2 })
	////	.build(&scene, meshEntity);

	//auto camera = MiniEngine::Camera::Builder()
	//	.setPosition({3, 0, 3 })
	//	.setAspectRatio((float)params.screenWidth / (float)params.screenHeight)
	//	.setNearFarPlane(0.1f, 1000.0f)
	//	.setFOV(45)
	//	.build(&scene, cameraEntity2);

	//auto light = MiniEngine::Light::Builder()
	//	.setPosition({ 0,0,0 })
	//	.setIntensity(1.0f)
	//	.build(&scene, mainLightEntity);

	//// todo, hides when x/z 0. Need to set forward vector etc.
	//scene.setCameraActive(camera);
	//
	////scene.addLight(light);
	//scene.setSkyboxActive(&skyBox);
	////scene.addLight(light);

	//engine.addSlider("Skybox", &(skyBox.rotation), 0.0f, 3.142f * 2);
	//engine.addSlider("Model", &(mesh->rotation.y), 0.0f, 360);

	//engine.addSlider("Light-X", &(light->position.x), -1, 1, [&]() { scene.addLight(light); });
	//engine.addSlider("Light-Y", &(light->position.y), -1, 1, [&]() { scene.addLight(light); });
	//engine.addSlider("Light-Z", &(light->position.z), -1, 1, [&]() { scene.addLight(light); });
	//engine.addSlider("Light-Intensity", &(light->intensity), 0, 10, [&]() { scene.addLight(light); });

	//float cameraRot = 0;
	//engine.addSlider("Camera-X", &(cameraRot), 0, 3.142f * 2, [&]() 
	//	{ 
	//		camera->position.x = 3 * cos(cameraRot);
	//		camera->position.z = 3 * sin(cameraRot);
	//		scene.setCameraActive(camera); 
	//		skyBox.rotation = cameraRot;
	//	});

	//float r = material.materialProperties[(int)MiniEngine::Material::PropertyType::Roughness];
	//float g = material.materialProperties[(int)MiniEngine::Material::PropertyType::Metallic];
	//engine.addSlider("Roughness", &r, 0, 1, [&]() { material.materialProperties.set((int)MiniEngine::Material::PropertyType::Roughness, r); });
	//engine.addSlider("Metallic", &g, 0, 1, [&]() { material.materialProperties.set((int)MiniEngine::Material::PropertyType::Metallic, g); });
	//engine.addCheckbox("post-process", params.enablePostProcess);

	engine.execute(&scene); //move to separate thread
}
