#version 450 core

layout (location = 0) out vec3 oColor; 
layout (location = 1) out vec3 oPosition; 
layout (location = 2) out vec3 oNormal; 
layout (location = 3) out vec3 oRoughness;

void main()
{
	oColor = vec3(1.0, 0.0, 0.0);
	oPosition = vec3(0.0, 1.0, 0.0);
	oNormal = vec3(0.0, 0.0, 1.0);
	oRoughness = vec3(1.0);
}