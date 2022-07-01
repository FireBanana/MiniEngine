#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include "core/Initializer.h"
#include "core/Shader.h"
#include "core/Object.h"
#include "core/Renderer.h"
#include "core/UniformGLData.h"
#include "core/Time.h"
#include "core/utils/Constants.h"

int main(void)
{
	Initializer ini;
	Renderer renderer;
	Camera cam;
	Time time;

	Shader standardShader(DIR "/shaders/vertex.vs", DIR "/shaders/fragment.fs");
	Shader shadowShader(DIR "/shaders/shadow.vs", DIR "/shaders/shadow.fs");

	Object cube(standardShader, Constants::Primitives::CubeVertices, 36, { 3, 3, 2 });
	Object plane(standardShader, Constants::Primitives::PlaneVertices, 6, { 3, 3, 2 });

	plane.Translate(0.0f, -1.0f, 0.0f);
	plane.Scale(20.0f, 1.0f, 20.0f);

	Light directionalLight{ glm::vec3(5.0f, 5.0f, 5.0f), LIGHT_TYPE::Direction };

	standardShader.SetUniform_f3("camPos", cam.m_CameraPosition.x, cam.m_CameraPosition.y, cam.m_CameraPosition.z);

#pragma region SHADOW_MAPPING
	cube.Translate(0, 0, 0);

	unsigned int shadowMap, shadowBuffer;

	glGenFramebuffers(1, &shadowBuffer);

	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	standardShader.SetUniform_i("shadowMap", 0);

	glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), directionalLight.m_Position);
	glm::mat4 lightView = glm::lookAt(directionalLight.m_Position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	UniformGLData::Get().RegisterShader(standardShader, "Matrices");

	UniformGLData::Get().
		UpdateBufferData(
			offsetof(UniformGLData::BufferStructure, UniformGLData::BufferStructure::lightPos),
			sizeof(directionalLight.m_Position), glm::value_ptr(directionalLight.m_Position)
		);
	
	UniformGLData::Get().UpdateBufferData(offsetof(UniformGLData::BufferStructure, UniformGLData::BufferStructure::projection), sizeof(cam.m_ProjectionMatrix), glm::value_ptr(cam.m_ProjectionMatrix));
	UniformGLData::Get().UpdateBufferData(offsetof(UniformGLData::BufferStructure, UniformGLData::BufferStructure::view), sizeof(cam.m_ViewMatrix), glm::value_ptr(cam.m_ViewMatrix));
	UniformGLData::Get().UpdateBufferData(offsetof(UniformGLData::BufferStructure, UniformGLData::BufferStructure::lightvp), sizeof(lightSpaceMatrix), glm::value_ptr(lightSpaceMatrix));

#pragma endregion

	glEnable(GL_DEPTH_TEST);

	renderer.AddObject(plane);
	renderer.AddObject(cube);
	renderer.AddLight(directionalLight);

	while (!glfwWindowShouldClose(ini.m_Window))
	{
		time.Update(glfwGetTime());

#pragma region SHADOW_MAPPING
		shadowShader.Use();
		glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);		

		glUniformMatrix4fv(
			glGetUniformLocation(shadowShader.GetShaderId(), "viewprojection"),
			1, GL_FALSE,
			glm::value_ptr(lightSpaceMatrix));

		glUniformMatrix4fv(
			glGetUniformLocation(shadowShader.GetShaderId(), "model"),
			1, GL_FALSE,
			glm::value_ptr(lightModel));

		renderer.Render(cam, shadowShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

		standardShader.Use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.Render(cam, standardShader);

		cube.Rotate(0, 0.87f * time.GetDeltaTime(), 0);

		glfwSwapBuffers(ini.m_Window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
