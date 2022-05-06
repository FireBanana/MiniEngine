#pragma once
#include <string>
#include <glad/glad.h>
#include<glm.hpp>
#include <iostream>

class Shader
{
public:

	Shader(const char* vPath, const char* fPath);

	inline void			Use()		  { glUseProgram(m_Id); }
	inline unsigned int GetShaderId() { return m_Id; }

	void SetUniform_f(std::string name, float value);
	void SetUniform_f3(std::string name, float x, float y, float z);
	void SetUniform_m(std::string name, glm::mat4& value);
	void SetUniform_i(std::string name, int value);

private:

	unsigned int m_Id;

	void CheckShader(unsigned int shader);
	void CheckProgram(unsigned int id);
};

