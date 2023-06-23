#version 450 core

layout (location = 0) in vec3 vPos;

uniform mat4 _view;
uniform mat4 _projection;

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

out vec3 fPos;

void main()
{
    fPos = vPos;
    vec4 pos = _projection * _view * vec4(vPos, 1.0);
    gl_Position = pos;
}