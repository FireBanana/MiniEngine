#include "Shader.h"
#include "Types.h"

#include<sstream>
#include <fstream>
#include <iostream>
#include<glm.hpp>
#include <gtc/type_ptr.hpp>

Shader::Shader(const char* vPath, const char* fPath)
{
	unsigned int vertex = 0, fragment = 0;

	std::ifstream vFile(vPath);

	std::stringstream vs;
	vs << vFile.rdbuf();
	std::string vSourceString = vs.str();
	const char* vSource = vSourceString.c_str();

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vSource, NULL);
	glCompileShader(vertex);

	CheckShader(vertex);

	//==========================================

	std::ifstream fFile(fPath);

	std::stringstream fs;
	fs << fFile.rdbuf();
	std::string fSourceString = fs.str();
	const char* fSource = fSourceString.c_str();

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fSource, NULL);
	glCompileShader(fragment);

	CheckShader(fragment);

	//==========================================

	m_Id = glCreateProgram();

	glAttachShader(m_Id, vertex);
	glAttachShader(m_Id, fragment);
	glLinkProgram(m_Id);

	CheckProgram(m_Id);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	glUseProgram(m_Id);

	SetDefaultMapLocations();
}

void Shader::SetUniform_f(std::string name, float value)
{
	Use();

	unsigned int location = glGetUniformLocation(m_Id, name.c_str());
	glUniform1f(location, value);
}

void Shader::SetUniform_f3(std::string name, float x, float y, float z)
{
	Use();

	unsigned int location = glGetUniformLocation(m_Id, name.c_str());
	glUniform3f(location, x, y, z);
}

void Shader::SetUniform_m(std::string name, glm::mat4& value)
{
	Use();

	unsigned int location = glGetUniformLocation(m_Id, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetUniform_m(std::string name, glm::mat4& value, Shader& shader)
{
	shader.Use();

	unsigned int location = glGetUniformLocation(m_Id, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetUniform_i(std::string name, int value)
{
	Use();

	unsigned int location = glGetUniformLocation(m_Id, name.c_str());
	glUniform1i(location, value);
}

void Shader::SetDefaultMapLocations()
{
	//This will run on all shaders, so all shaders will have these maps
	SetUniform_i("shadowMap", 0);
	SetUniform_i("textureMap", 1);
	SetUniform_i("normalMap", 2);
	SetUniform_i("roughnessMap", 3);
}

void Shader::CheckShader(unsigned int shader)
{
	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void Shader::CheckProgram(unsigned int id)
{
	int  success;
	char infoLog[1024];
	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(id, 1024, NULL, infoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR" << std::endl;
		std::cout << infoLog;
	}
}
