#version 450 core

#define PI 3.142

in vec2 oUv;

layout (std140, binding = 0) uniform CameraBlock
{
	mat4 view;
	mat4 projection;
    vec3 cameraPos;
};

layout (std140, binding = 1) uniform LightBlock
{
	vec3 lightPos1;
    float lightIntensity1;
};

layout (std140, binding = 2) uniform MaterialBlock
{
    float roughness;
    float metallic;
};

uniform sampler2D _Diffuse;
uniform sampler2D _Position;
uniform sampler2D _Normal;
uniform sampler2D _Roughness;

uniform float _Roughness;
uniform float _Metallic;

layout (location = 0) out vec4 oAccum;

float D_GGX(float NoH, float a) 
{
    float a2 = a * a * a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

vec3 F_Schlick(vec3 f0, float VoH) 
{
    return f0 + (1. - f0) * pow(2., ( -5.55473 * VoH - 6.98316) * VoH);
    //return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) 
{
    float k = (( a + 1. ) * ( a + 1. )) / 8.;
    float g1 = ( NoV ) / ( NoV * ( 1. - k ) + k );
    float g2 = ( NoL ) / ( NoL * ( 1. - k ) + k );
    return g1 * g2;

    // float a2 = a * a;
    // float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    // float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    // return 0.5 / (GGXV + GGXL);
}

float Fd_Lambert() 
{
    return 1.0 / PI;
}

vec3 BRDF(vec3 albedo, vec3 v, vec3 n, vec3 l, float a) 
{
    vec3 h = normalize(v + l);

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);
    float VoH = clamp(dot(v, h), 0.0, 1.0);

    // perceptually linear roughness to roughness (see parameterization)
    float roughness = a * a;

    float D = D_GGX(NoH, a);
    vec3  F = F_Schlick(vec3(0.5), VoH);
    float V = V_SmithGGXCorrelated(NoV, NoL, roughness);

    // specular BRDF
    vec3 Fr = ((D * V) * F) / (4 * NoV * NoL);

    vec3 li = ( albedo / PI );

    float illuminance = NoL * 2.; //intensity

    return (Fr + li) * illuminance;
}

void main()
{    
    vec3 albedo = texture(_Diffuse, oUv).xyz;
    vec3 position = texture(_Position, oUv).xyz;
    vec3 normal = texture(_Normal, oUv).xyz;

    vec3 viewDirClipSpace = cameraPos - position;
    
    vec3 viewDir = normalize(viewDirClipSpace).xyz;
    vec3 lightDir = normalize(lightPos1 - position);
    vec3 halfv = normalize( viewDir + lightDir );

    vec3 c = BRDF(albedo, viewDir, normalize(normal), lightDir, _Rougness);

    //c *= 1.0 / pow( length(lightPos1 - position), 2. ); //attenuation

    oAccum = vec4(c,1.0);
}