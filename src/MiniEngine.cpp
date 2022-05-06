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

	glm::vec3 lightPos{ 5.0f, 5.0f, 0.0f };

	Shader standardShader(
		DIR "/shaders/vertex.vs",
		DIR "/shaders/fragment.fs");

	UniformGLData::Get().RegisterShader(standardShader, "Matrices");
	UniformGLData::Get().UpdateBufferData(sizeof(cam.m_ViewMatrix), glm::value_ptr(cam.m_ViewMatrix));
	UniformGLData::Get().UpdateBufferData(sizeof(cam.m_ProjectionMatrix), glm::value_ptr(cam.m_ProjectionMatrix));
	UniformGLData::Get().UpdateBufferData(sizeof(lightPos), glm::value_ptr(lightPos));

	Object cube(standardShader, Constants::Primitives::CubeVertices, 36,  { 3, 3, 2 });
	Object plane(standardShader, Constants::Primitives::PlaneVertices, 6, { 3, 3, 2 });

	plane.Translate(0.0f, -1.0f, 0.0f);
	// TODO : Add scaling (shader will need to update)

	Light directionalLight{ glm::vec3(0.0f, 0.0f, 0.0f), LIGHT_TYPE::Direction };

	standardShader.SetUniform_f3("camPos", cam.m_CameraPosition.x, cam.m_CameraPosition.y, cam.m_CameraPosition.z);

	renderer.AddObject(plane);
	renderer.AddObject(cube);
	renderer.AddLight(directionalLight);

	standardShader.Use();

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(ini.m_Window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		time.Update(glfwGetTime());		

		renderer.Render(cam);

		cube.Rotate(0,0.77f * time.GetDeltaTime(), 0);

		glfwSwapBuffers(ini.m_Window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
