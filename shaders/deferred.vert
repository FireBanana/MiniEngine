#version 450 core

layout (location = 0) in vec2 iPos;
layout (location = 1) in vec2 iUv;

out vec2 fUv;

void main()
{
	fUv = iUv;
	gl_Position = vec4(iPos, 0.0, 1.0);
}