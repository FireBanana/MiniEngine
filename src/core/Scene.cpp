#include "Scene.h"
#include "../components/RenderableComponent.h"
#include "../components/CameraComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "Material.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene(Engine* engine) :
	mEngine(engine)
{

}

Entity* Scene::createEntity()
{
	mEntityDatabase.push(Entity{});
	return &(mEntityDatabase.getLast());
}

RenderableComponent* Scene::createRenderable(const Renderable::Builder* builderResults, Entity* entity)
{
	RenderableComponent m{};
	m.entityHandle = entity;
	m.buffer = builderResults->getBuffer();
	m.indices = builderResults->getIndices();
	m.attributes = builderResults->getAttributes();

	if (builderResults->getMaterial() == nullptr)
	{
		m.shader = *(mEngine->getShaderRegistry()->getDeferredShader());
	}
	else
	{
		m.shader = *(builderResults->getMaterial()->getShader());
		m.textures = builderResults->getMaterial()->getTextureReference();
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

CameraComponent* Scene::createCamera(const Camera::Builder* builderResults, Entity* entity)
{
	CameraComponent c{};
	c.entityHandle = entity;
	c.position = builderResults->getPosition();
	c.aspectRatio = builderResults->getAspectRatio();
	c.farPlane = builderResults->getFarPlane();
	c.fov = builderResults->getFov();
	c.nearPlane = builderResults->getNearPlane();

	mCameraComponentDatabase.push(std::move(c));
	return &(mCameraComponentDatabase.getLast());
}

void Scene::setCameraActive(const CameraComponent* camera)
{
	glm::vec3 pos = { camera->position.x, camera->position.y, camera->position.z };

	auto view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto projection = 
		glm::perspective(
			glm::radians(camera->fov),
			camera->aspectRatio, camera->nearPlane,
			camera->farPlane);

	struct MatrixBlock
	{
		glm::mat4 view;
		glm::mat4 projection;
	} t1;

	t1.view = view;
	t1.projection = projection;

	mEngine->getShaderRegistry()->setActiveUniformBuffer("MatrixBlock", sizeof(MatrixBlock), &t1);
}
