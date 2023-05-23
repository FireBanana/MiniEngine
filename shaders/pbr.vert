#version 450 core

out vec2 oUv;

layout (std140, binding = 0) uniform MatrixBlock
{
	mat4 view;
	mat4 projection;
};

layout (location = 0) in vec2 iPos;
layout (location = 1) in vec2 iUv;

void main()
{
	oUv = iUv;
    gl_Position = vec4(iPos, 0., 0.);
}