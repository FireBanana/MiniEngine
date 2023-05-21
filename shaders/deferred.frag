#version 450 core

in vec2 fUv;
in vec3 fNormal;

uniform sampler2D iImg;

layout (location = 0) out vec3 oDiffuse; 
layout (location = 1) out vec3 oPosition; 
layout (location = 2) out vec3 oNormal; 
layout (location = 3) out vec3 oRoughness;

void main()
{
	vec3 v = normalize(fNormal);

	oDiffuse = texture(iImg, fUv).xyz;
	oPosition = vec3(0.0, 1.0, 0.0);
	oNormal = vec3(v.x, v.y, v.z);
	oRoughness = vec3(1.0);
}