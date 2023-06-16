#version 450 core

layout (location = 0) in vec3 vPos;

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
    vec4 pos = projection * view * vec4(vPos, 1.0);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
}