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
			m.textures = builderResults->getMaterialInstance()->textureReference;
			m.materialProperties = builderResults->getMaterialInstance()->materialProperties;
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

	void Scene::setCameraActive(const Components::CameraComponent* camera)
	{
		glm::vec3 pos = { camera->position.x, camera->position.y, camera->position.z };

		auto view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		auto projection =
			glm::perspective(
				glm::radians(camera->fov),
				camera->aspectRatio, camera->nearPlane,
				camera->farPlane);

		struct CameraUniformBlock
		{
			glm::mat4 view;
			glm::mat4 projection;
			glm::vec3 cameraPos;
		} t1;

		t1.view = view;
		t1.projection = projection;
		t1.cameraPos = { camera->position.x, camera->position.y, camera->position.z };

		mEngine->getGlobalBufferRegistry()->createNewBinding(sizeof(CameraUniformBlock), &t1, 0);
		mEngine->getShaderRegistry()->bindGlobalBufferToAll("CameraBlock", 0);
	}
	
	void Scene::addLight(const Components::LightComponent* light)
	{
		struct LightUniformBlock
		{
			glm::vec3 lightPos1;
			float lightIntensity1;
		} l1;

		l1.lightPos1 = glm::vec3{ light->position.x, light->position.y, light->position.z };

		mEngine->getGlobalBufferRegistry()->createNewBinding(sizeof(LightUniformBlock), &l1, 1);
		mEngine->getShaderRegistry()->bindGlobalBufferToAll("LightBlock", 1);
	}
}