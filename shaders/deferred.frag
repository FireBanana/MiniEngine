#version 450 core

in vec2 fUv;

uniform sampler2D iImg;

layout (location = 0) out vec3 oDiffuse; 
layout (location = 1) out vec3 oPosition; 
layout (location = 2) out vec3 oNormal; 
layout (location = 3) out vec3 oRoughness;

void main()
{
	oDiffuse = texture(iImg, fUv).xyz;//vec3(fUv.x, fUv.y, 0.);
	oPosition = vec3(0.0, 1.0, 0.0);
	oNormal = vec3(0.0, 0.0, 1.0);
	oRoughness = vec3(1.0);
}