#pragma once

#include <vector>
#include "Component.h"
#include "../core/utils/ComponentArray.h"
#include "../core/Texture.h"
#include "../core/Shader.h"
#include "../core/Skybox.h"

namespace MiniEngine::Components
{
	struct SkyboxComponent : public Component
	{
		Skybox::SkyboxType skyboxType;
		Texture			   mainTexture;

#ifdef USING_OPENGL
		unsigned int			  vaoId;
#endif
	};
}