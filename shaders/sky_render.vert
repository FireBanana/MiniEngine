#version 450 core
layout (location = 0) in vec3 iPos;

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

uniform mat4 _projection;
out vec3 localPos;

void main()
{
    localPos = iPos;

    mat4 rotView = mat4(mat3(view)); // remove translation from the view matrix
    vec4 clipPos = _projection * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}