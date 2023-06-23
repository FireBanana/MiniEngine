#version 450 core

in vec3 fPos;

uniform sampler2D _HdrMap;
uniform layout(binding=0,rgba16f) writeonly image2D cubemapSlice;

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

    //oAccum = vec4(color, 1.0);
}