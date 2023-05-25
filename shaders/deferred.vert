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
out vec3 fPosition;

void main()
{
	fUv = iUv;
	fNormal = iNormal;
	fPosition = iPos; //mult with model mat
	gl_Position = projection * view * vec4(iPos, 1.0);
}