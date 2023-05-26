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
    const vec3 lPos = vec3(-3.0,4.0,-5.0);
    vec3 position = texture(_Position, oUv).xyz;
    vec3 normal = texture(_Normal, oUv).xyz;

    float a = 0.1 * length(normal);    
    a += max(dot(normalize(normal), normalize(position - lPos)), 0.);
    vec4 res = vec4(a,a,a,1.);

    oAccum = texture(_Diffuse, oUv) * a;
}