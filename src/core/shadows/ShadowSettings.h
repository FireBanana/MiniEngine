#pragma once

#include "../Shader.h"
#include "../Light.h"
#include "../Renderer.h"
#include "../Camera.h"
#include "DirectionalShadowPass.h"

class ShadowSettings
{
public:

	Light& SceneLight;
	Shader ShadowShader;

	ShadowSettings(Light& light);

	void ShadowPass(Renderer* renderer, Camera* camera);

private:

	unsigned int m_ShadowMapId;
	unsigned int m_ShadowBuffer;	

	DirectionalShadowPass m_DirectionalShadowPass;

};

