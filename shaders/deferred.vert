#version 450 core

layout (std140, binding = 0) uniform MatrixBlock
{
	mat4 view;
	mat4 projection;
};

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iUv;
layout (location = 2) in vec3 iNormal;

out vec2 fUv;
out vec3 fNormal;

void main()
{
	fUv = iUv;
	fNormal = iNormal;
	gl_Position = projection * view * vec4(iPos, 1.0);
}