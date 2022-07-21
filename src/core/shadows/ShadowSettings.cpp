#include "ShadowSettings.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

ShadowSettings::ShadowSettings(Light& light) 
	: SceneLight(light), ShadowShader(DIR "/shaders/shadow.vs", DIR "/shaders/shadow.fs"),
	  m_DirectionalShadowPass(this)
{
	
}

void ShadowSettings::ShadowPass(Renderer* renderer, Camera* camera)
{
	m_DirectionalShadowPass.Pass(renderer, camera);
}
