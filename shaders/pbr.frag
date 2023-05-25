#version 450 core

in vec2 oUv;

layout (std140, binding = 0) uniform MatrixBlock
{
	mat4 view;
	mat4 projection;
};

uniform sampler2D _Diffuse;
uniform sampler2D _Position;
uniform sampler2D _Normal;
uniform sampler2D _Roughness;

layout (location = 0) out vec4 oAccum;

void main()
{
    // Egzample light position
    const vec3 lPos = vec3(3.0,4.0,5.0);

    oAccum = texture(_Position, oUv);
}