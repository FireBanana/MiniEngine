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

in vec2 fUv;
in vec3 fNormal;
in vec3 fPosition;

uniform sampler2D _Diffuse;
uniform sampler2D _Position;
uniform sampler2D _Normal;
uniform sampler2D _Roughness;

uniform float _baseRoughness;
uniform float _baseMetallic;

layout (location = 1) out vec4 oDiffuse; 
layout (location = 2) out vec4 oPosition; 
layout (location = 3) out vec4 oNormal; 
layout (location = 4) out vec4 oRoughness;

void main()
{
	vec3 v = normalize(fNormal);

	oDiffuse = texture(_Diffuse, fUv);
	oPosition = vec4(normalize(fPosition), 0.0);
	oNormal = vec4(v.x, v.y, v.z, 0.0);
	oRoughness = vec4(_baseRoughness);
}