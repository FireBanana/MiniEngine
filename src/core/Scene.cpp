#include "Scene.h"
#include "../components/RenderableComponent.h"
#include "../components/CameraComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "Material.h"
#include "Loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MiniEngine
{

	Scene::Scene(Engine* engine) :
		mEngine(engine)
	{
		mActiveSkybox = nullptr;
	}

	Entity* Scene::createEntity()
	{
		mEntityDatabase.push(Entity{});
		return &(mEntityDatabase.getLast());
	}

	Components::RenderableComponent* Scene::createRenderable(const Renderable::Builder* builderResults, Entity* entity) //TODO: Move this elsewhere
	{
		Components::RenderableComponent m{};
		m.entityHandle = entity;

		if (builderResults->getModelPath() != nullptr)
		{
			auto results = MiniTools::ModelLoader::load(builderResults->getModelPath());

			m.buffer = std::move(results.models[0].bufferData);
			m.indices = std::move(results.models[0].indices);
			m.attributes = std::move(results.models[0].vertexAttributeSizes);
		}
		else
		{
			m.buffer = builderResults->getBuffer();
			m.indices = builderResults->getIndices();
			m.attributes = builderResults->getAttributes();
		}

		if (builderResults->getMaterialInstance() == nullptr)
		{
			m.shader = *(mEngine->getShaderRegistry()->getDeferredShader());
		}
		else
		{
			m.shader = *(builderResults->getMaterialInstance()->shader);
			m.materialInstance = builderResults->getMaterialInstance();
		}

#ifdef USING_OPENGL

		unsigned int stride = 0;

		for (auto i = 0; i < m.attributes.size(); ++i)
		{
			stride += m.attributes[i];
		}

		m.stride = stride;

		mEngine->getOpenGlDriver()->setupMesh(&m);

#endif // USING_OPENGL

		mRenderableComponentDatabase.push(std::move(m));
		return &(mRenderableComponentDatabase.getLast());
	}

	Components::CameraComponent* Scene::createCamera(const Camera::Builder* builderResults, Entity* entity)
	{
		Components::CameraComponent c{};
		c.entityHandle = entity;
		c.position = builderResults->getPosition();
		c.aspectRatio = builderResults->getAspectRatio();
		c.farPlane = builderResults->getFarPlane();
		c.fov = builderResults->getFov();
		c.nearPlane = builderResults->getNearPlane();

		mCameraComponentDatabase.push(std::move(c));
		return &(mCameraComponentDatabase.getLast());
	}

	Components::LightComponent* Scene::createLight(const Light::Builder* builderResults, Entity* entity)
	{
		Components::LightComponent l{};
		l.entityHandle = entity;
		l.position = builderResults->getPosition();
		l.intensity = builderResults->getIntensity();

		mLightComponentDatabase.push(std::move(l));
		return &(mLightComponentDatabase.getLast());
	}

	Components::SkyboxComponent Scene::createSkybox(const Skybox::Builder* builderResults, Entity* entity)
	{
		Components::SkyboxComponent s{};
		s.mainTexture = builderResults->getTexture();
		s.skyboxType = Skybox::SkyboxType::Skybox;

		mEngine->getOpenGlDriver()->setupSkybox(&s);

		return s;
	}

	void Scene::setCameraActive(const Components::CameraComponent* camera)
	{
		glm::vec3 pos = { camera->position.x, camera->position.y, camera->position.z };
		auto sceneBlock = mEngine->getGlobalBufferRegistry()->getSceneBlockInstance();

		auto view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		auto projection =
			glm::perspective(
				glm::radians(camera->fov),
				camera->aspectRatio, camera->nearPlane,
				camera->farPlane);

		sceneBlock->view = view;
		sceneBlock->projection = projection;
		sceneBlock->cameraPos = { camera->position.x, camera->position.y, camera->position.z };

		mEngine->getGlobalBufferRegistry()->updateUniformData(
			GlobalBufferRegistry::BlockType::SceneBlock,
			offsetof(GlobalBufferRegistry::SceneBlock, GlobalBufferRegistry::SceneBlock::view),
			sizeof(sceneBlock->view) + sizeof(sceneBlock->projection) + sizeof(sceneBlock->cameraPos), &(sceneBlock->view));

		mEngine->getShaderRegistry()->bindGlobalBufferToAll("SceneBlock", 0);
	}
	
	void Scene::addLight(const Components::LightComponent* light)
	{
		auto sceneBlock = mEngine->getGlobalBufferRegistry()->getSceneBlockInstance();
		sceneBlock->lightPos1 = light->position;//glm::vec3{ light->position.x, light->position.y, light->position.z };

		mEngine->getGlobalBufferRegistry()->updateUniformData(
			GlobalBufferRegistry::BlockType::SceneBlock,
			offsetof(GlobalBufferRegistry::SceneBlock, GlobalBufferRegistry::SceneBlock::lightPos1),
			sizeof(sceneBlock->lightPos1), &(sceneBlock->lightPos1));

		mEngine->getShaderRegistry()->bindGlobalBufferToAll("SceneBlock", 0);
	}

	void Scene::setSkyboxActive(Components::SkyboxComponent* skybox)
	{
		mActiveSkybox = skybox;
	}
}