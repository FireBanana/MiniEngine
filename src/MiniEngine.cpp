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
#include "core/ShadowSettings.h"
#include "core/utils/Constants.h"
 
int main(void)
{
	Light directionalLight{glm::vec3(5.0f, 5.0f, 5.0f)};

	Initializer	   ini;
	Renderer	   renderer;
	Camera		   cam;
	Time		   time;
	
	Shader standardShader(DIR "/shaders/vertex.vs", DIR "/shaders/fragment.fs");

	Object cube(standardShader, Constants::Primitives::CubeVertices, 36, { 3, 3, 2, 3 }); //TODO add auto attrib deduction
	Object plane(standardShader, Constants::Primitives::PlaneVertices, 6, { 3, 3, 2, 3 });

	plane.Translate(0.0f, -1.0f, 0.0f);
	plane.Scale(2.0f, 1.0f, 2.0f);

	ShadowSettings shadowSettings(directionalLight);
	standardShader.SetUniform_i("shadowMap", 0);

	UniformGLData::Get().RegisterShader(standardShader, "Matrices");

	UniformGLData::Get()
		.UpdateBufferData(
			offsetof(UniformGLData::BufferStructure, UniformGLData::BufferStructure::lightPos),
			sizeof(directionalLight.GetVPMatrix()), glm::value_ptr(directionalLight.GetPosition())
		);
	UniformGLData::Get()
		.UpdateBufferData(
			offsetof(UniformGLData::BufferStructure, UniformGLData::BufferStructure::projection),
			sizeof(cam.m_ProjectionMatrix), glm::value_ptr(cam.m_ProjectionMatrix)
		);
	UniformGLData::Get()
		.UpdateBufferData(
			offsetof(UniformGLData::BufferStructure, UniformGLData::BufferStructure::view),
			sizeof(cam.m_ViewMatrix), glm::value_ptr(cam.m_ViewMatrix)
		);
	UniformGLData::Get()
		.UpdateBufferData(
			offsetof(UniformGLData::BufferStructure, UniformGLData::BufferStructure::lightvp),
			sizeof(directionalLight.GetVPMatrix()), glm::value_ptr(directionalLight.GetVPMatrix())
		);

	glEnable(GL_DEPTH_TEST);

	renderer.AddObject(plane);
	renderer.AddObject(cube);
	renderer.AddLight(directionalLight);

	//Move to uniform?
	standardShader.SetUniform_f3("camPos", cam.m_CameraPosition.x, cam.m_CameraPosition.y, cam.m_CameraPosition.z);	

	cube.AddTexture(R"(..\assets\color.jpg)", GL_TEXTURE1);
	standardShader.SetUniform_i("textureMap", 1);

	cube.AddTexture(R"(..\assets\normal.jpg)", GL_TEXTURE2);
	standardShader.SetUniform_i("normalMap", 2);

	while (!glfwWindowShouldClose(ini.m_Window))
	{
		time.Update(glfwGetTime());

		shadowSettings.ShadowPass(&renderer, &cam);

		standardShader.Use();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.Render(cam, standardShader);

		cube.Rotate(0.87f * time.GetDeltaTime(), 0.0f, 0);
		plane.Rotate(0.0f, 0.87f * time.GetDeltaTime(), 0.0f);

		glfwSwapBuffers(ini.m_Window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
