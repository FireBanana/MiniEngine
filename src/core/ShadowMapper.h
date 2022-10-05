#pragma once

#include "Shader.h"
#include "Light.h"
#include "Camera.h"

class Renderer; //TODO: remove forward declarations

class ShadowMapper
{
public:

	ShadowMapper();

	void ShadowPass(Renderer* renderer, Camera* camera, Light* light);
	void BindTexture() const;

private:

	unsigned int m_ShadowMapId;
	unsigned int m_ShadowBuffer;

	Shader m_ShadowShader;
};

