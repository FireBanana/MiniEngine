#version 450 core

in vec2 oUv;

layout (std140, binding = 0) uniform MatrixBlock
{
	mat4 view;
	mat4 projection;
};

// Attach all maps
uniform sampler2D iImg;

layout (location = 0) out vec4 oAccum;
layout (location = 1) out vec4 oDiffuse; 
layout (location = 2) out vec4 oPosition; 
layout (location = 3) out vec4 oNormal; 
layout (location = 4) out vec4 oRoughness;

void main()
{
    // Egzample light position
    const vec3 lPos = vec3(3.0,4.0,5.0);
    vec4 color = texture(iImg, oUv);


    oAccum = texture(iImg, oUv);
}