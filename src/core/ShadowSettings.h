#pragma once

#include "Shader.h"
#include "Light.h"
#include "Renderer.h"
#include "Camera.h"

class ShadowSettings
{
public:

	ShadowSettings(Light light);

	void ShadowPass(Renderer* renderer, Camera* camera);
	void BindTexture() const;

private:

	unsigned int m_ShadowMapId;
	unsigned int m_ShadowBuffer;

	Shader m_ShadowShader;
	Light m_Light;

};

