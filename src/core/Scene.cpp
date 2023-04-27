#include "Scene.h"
#include "../components/MeshComponent.h"
#include "../components/CameraComponent.h"
#include "Entity.h"
#include "Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene(Engine* engine) :
	mEngine(engine)
{

}

Entity* Scene::createEntity()
{
	mEntityDatabase.push_back(Entity{});
	return &(mEntityDatabase.back());
}

MeshComponent* Scene::createMesh(const Mesh::Builder* builderResults, Entity* entity)
{
	MeshComponent m{};
	m.entityHandle = entity;
	m.buffer = builderResults->getBuffer();
	m.indices = builderResults->getIndices();
	m.attributes = builderResults->getAttributes();
	
#ifdef USING_OPENGL

	unsigned int stride = 0;

	for (auto i = 0; i < m.attributes.size(); ++i)
	{
		stride += m.attributes[i];
	}

	m.stride = stride;

	mEngine->getOpenGlDriver()->setupMesh(&m);

#endif // USING_OPENGL

	mMeshComponentDatabase.push_back(std::move(m));
	return &mMeshComponentDatabase.back();
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

	mCameraComponentDatabase.push_back(std::move(c));
	return &mCameraComponentDatabase.back();
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