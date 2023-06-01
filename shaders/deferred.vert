#version 450 core

layout (std140, binding = 0) uniform MatrixBlock
{
	mat4 view;
	mat4 projection;
	vec3 cameraPos;
};

uniform mat4 _model;
uniform mat4 _rotation_only;

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iUv;
layout (location = 2) in vec3 iNormal;

out vec2 fUv;
out vec3 fNormal;
out vec3 fPosition;

void main()
{
	fUv = iUv;
	fNormal = (_rotation_only * vec4(iNormal, 1.0)).xyz;
	fPosition = (_model * vec4(iPos, 1.0)).xyz;
	gl_Position = projection * view * _model * vec4(iPos, 1.0);
}