#version 450 core

layout (location = 0) out vec4 oAccum;

in vec3 fPos;

uniform sampler2D _HdrMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sampleHdr(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = sampleHdr(normalize(fPos));
    vec3 color = texture(_HdrMap, uv).rgb;

    oAccum = vec4(color, 1.0);
}