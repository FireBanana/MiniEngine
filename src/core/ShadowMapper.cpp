#include "ShadowMapper.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

constexpr unsigned int SHADOW_RESOLUTION = 4096;

ShadowMapper::ShadowMapper() 
	: m_ShadowShader(DIR "/shaders/shadow.vs", DIR "/shaders/shadow.fs")
{
	glGenFramebuffers(1, &m_ShadowBuffer);

	glGenTextures(1, &m_ShadowMapId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMapId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_RESOLUTION, SHADOW_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMapId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapper::ShadowPass(Renderer *renderer, Camera *camera, Light *light) {
	m_ShadowShader.Use();
	BindTexture();

	glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowBuffer);
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(
		glGetUniformLocation(m_ShadowShader.GetShaderId(), "viewprojection"),
		1, GL_FALSE,
		glm::value_ptr(light->GetVPMatrix()));

	glUniformMatrix4fv(
		glGetUniformLocation(m_ShadowShader.GetShaderId(), "model"),
		1, GL_FALSE, glm::value_ptr(light->GetModelMatrix()));

	renderer->Render(*camera, m_ShadowShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 800, 600);
}

void ShadowMapper::BindTexture() const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMapId);
}
