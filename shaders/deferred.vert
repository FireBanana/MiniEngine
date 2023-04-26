#version 450 core

layout (std140, binding = 0) uniform MatrixBlock
{
	mat4 view;
	mat4 projection;
	vec2 offset;
};

layout (location = 0) in vec2 iPos;
layout (location = 1) in vec2 iUv;

out vec2 fUv;

void main()
{
	fUv = iUv;
	gl_Position = projection * view * vec4(iPos, 0.0, 1.0);
}