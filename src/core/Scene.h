#pragma once

#include <vector>

class Entity;

class Scene
{
public:

	Scene();

	void addEntity(Entity* entity);

private:

	std::vector<Entity*> mSceneEntities;

};