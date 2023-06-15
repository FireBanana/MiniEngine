#include "Skybox.h"
#include "Scene.h"

namespace MiniEngine
{
    Skybox::Builder& Skybox::Builder::setTexture(Texture tex)
    {
        mTexture = tex;
        return *this;
    }

    Components::SkyboxComponent* Skybox::Builder::build(Scene* scene, Entity* entity)
    {
        return scene->createSkybox(this, entity);
    }
}