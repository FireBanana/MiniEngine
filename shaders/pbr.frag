#version 450 core

in vec2 oUv;

layout (std140, binding = 0) uniform MatrixBlock
{
	mat4 view;
	mat4 projection;
};

// Attach all maps
uniform sampler2D diffuse_map;
uniform sampler2D position_map;
uniform sampler2D normal_map;
uniform sampler2D roughness_map;

layout (location = 0) out vec4 oAccum;

void main()
{
    // Egzample light position
    const vec3 lPos = vec3(3.0,4.0,5.0);

    oAccum = texture(normal_map, oUv);
}