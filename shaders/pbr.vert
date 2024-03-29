#version 450 core

out vec2 oUv;

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

layout (location = 0) in vec2 iPos;
layout (location = 1) in vec2 iUv;

void main()
{
	oUv = iUv;
    gl_Position = vec4(iPos, 0., 1.);
}