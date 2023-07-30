#version 450 core

layout (std140, binding = 0) uniform SceneBlock
{
	mat4 view;
	mat4 projection;
    vec3 cameraPos;

	vec3 lightPos1;
    float lightIntensity1;

    float roughness;
    float metallic;
};

uniform mat4 _model;

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iUv;
layout (location = 2) in vec3 iNormal;
layout (location = 3) in vec4 iTangent;

out vec2 fUv;
out vec3 fNormal;
out vec3 fPosition;
out vec4 fTangent;

void main()
{
	fUv = iUv;
	fNormal = (mat4(mat3(_model)) * vec4(iNormal, 1.0)).xyz;
	fPosition = (_model * vec4(iPos, 1.0)).xyz;
	fTangent = iTangent;
	gl_Position = projection * view * _model * vec4(iPos, 1.0);
}