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
#include "core/ShadowSettings.h"
 
int main(void)
{
	Initializer	   ini;
	Renderer	   renderer;
	Camera		   cam;
	Time		   time;

	Shader standardShader(DIR "/shaders/vertex.vs", DIR "/shaders/fragment.fs");

	#pragma region CALCULATE_TANGENTS_FOR_CUBE

	for(int i = 0 ; i < Constants::Primitives::CubeVertices.size(); i += 3)
	{
		auto& vArray = Constants::Primitives::CubeVertices;
		auto& v0 = vArray[i  ];
		auto& v1 = vArray[i+1];
		auto& v2 = vArray[i+2];

		auto edge1 = v1.position - v0.position;
		auto edge2 = v2.position - v0.position;

		auto deltaU1 = v1.texCoords.x - v0.texCoords.x;
		auto deltaV1 = v1.texCoords.y - v0.texCoords.y;
		auto deltaU2 = v2.texCoords.x - v0.texCoords.x;
		auto deltaV2 = v2.texCoords.y - v0.texCoords.y;

		auto f = 1.0f / ( deltaU1 * deltaV2 - deltaU2 * deltaV1 );

		auto tangent = glm::vec3
		{
			f * (deltaV2 * edge1.x - deltaV1 * edge2.x),
			f * (deltaV2 * edge1.y - deltaV1 * edge2.y),
			f * (deltaV2 * edge1.z - deltaV1 * edge2.z)
		};

		v0.tangent = normalize(tangent);
		v1.tangent = normalize(tangent);
		v2.tangent = normalize(tangent);
	}

	#pragma endregion

	Object cube(standardShader, Constants::Primitives::CubeVertices, 36, { 3, 3, 2, 3 });
	//Object plane(standardShader, Constants::Primitives::PlaneVertices, 6, { 3, 3, 2 });

	//plane.Translate(0.0f, -1.0f, 0.0f);
	//plane.Scale(20.0f, 1.0f, 20.0f);

	Light directionalLight{ glm::vec3(5.0f, 5.0f, 5.0f) };

	directionalLight.m_ModelMatrix = glm::translate(glm::mat4(1.0f), directionalLight.m_Position);
	glm::mat4 lightView = glm::lookAt(directionalLight.m_Position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	directionalLight.m_VPMatrix = lightProjection * lightView;

	ShadowSettings shadowSettings(directionalLight);
	standardShader.SetUniform_i("shadowMap", 0);

	UniformGLData::Get().RegisterShader(standardShader, "Matrices");

	UniformGLData::Get()
		.UpdateBufferData(
			offsetof(UniformGLData::BufferStructure, UniformGLData::BufferStructure::lightPos),
			sizeof(directionalLight.m_Position), glm::value_ptr(directionalLight.m_Position)
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
			sizeof(directionalLight.m_VPMatrix), glm::value_ptr(directionalLight.m_VPMatrix)
		);

	glEnable(GL_DEPTH_TEST);

	//renderer.AddObject(plane);
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

		glfwSwapBuffers(ini.m_Window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
