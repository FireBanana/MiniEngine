#include "DirectionalShadowPass.h"
#include "ShadowSettings.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

DirectionalShadowPass::DirectionalShadowPass(ShadowSettings* settings)
	: IShadowPass(), m_ShadowSettings(settings)
{
	Setup();
}

void DirectionalShadowPass::Setup()
{
	constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glGenFramebuffers(1, &ShadowBuffer);

	glGenTextures(1, &ShadowMapId);
	glBindTexture(GL_TEXTURE_2D, ShadowMapId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, ShadowBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowMapId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DirectionalShadowPass::Pass(Renderer* renderer, Camera* camera)
{
	m_ShadowSettings->ShadowShader.Use();

	glBindFramebuffer(GL_FRAMEBUFFER, ShadowBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(
		glGetUniformLocation(m_ShadowSettings->ShadowShader.GetShaderId(), "viewprojection"),
		1, GL_FALSE,
		glm::value_ptr(m_ShadowSettings->SceneLight.VPMatrix));

	glUniformMatrix4fv(
		glGetUniformLocation(m_ShadowSettings->ShadowShader.GetShaderId(), "model"),
		1, GL_FALSE,
		glm::value_ptr(m_ShadowSettings->SceneLight.ModelMatrix));

	renderer->Render(*camera, m_ShadowSettings->ShadowShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
