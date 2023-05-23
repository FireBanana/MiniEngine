#version 450 core

in vec2 fUv;
in vec3 fNormal;

uniform sampler2D iImg;

layout (location = 1) out vec4 oDiffuse; 
layout (location = 2) out vec4 oPosition; 
layout (location = 3) out vec4 oNormal; 
layout (location = 4) out vec4 oRoughness;

void main()
{
	vec3 v = normalize(fNormal);

	oDiffuse = texture(iImg, fUv);
	oPosition = vec4(0.0, 1.0, 0.0, 0.0);
	oNormal = vec4(v.x, v.y, v.z, 0.0);
	oRoughness = vec4(1.0);
}