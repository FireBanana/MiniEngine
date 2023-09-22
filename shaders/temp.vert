#version 450 core

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iUv;

layout (location = 0) out vec2 fUv;
layout (location = 1) out vec3 fPosition;

void main()
{
	fUv = vec2(1.);//iUv;
	fPosition = vec3(1.);//iPos;
	gl_Position = vec4(1.);//vec4(iPos,1.);
}