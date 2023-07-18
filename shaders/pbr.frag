#version 450 core

#define PI 3.142

in vec2 oUv;

layout (std140, binding = 0) uniform SceneBlock
{
	mat4 view;
	mat4 projection;
    vec3 cameraPos;

	vec3 lightPos1;
    float lightIntensity1;

    float roughness;
    float metallic;
};

uniform sampler2D _Diffuse;
uniform sampler2D _Position;
uniform sampler2D _Normal;
uniform sampler2D _Roughness;

uniform samplerCube _IrradianceMap;
uniform samplerCube _environmentMap;

layout (location = 0) out vec4 oAccum;

float D_GGX(float roughness, float NoH) 
{
    float a = roughness * roughness;
    float d = pow(NoH, 2.) * (a - 1.);

    float r = a / (PI * pow( d + 1. , 2.) );
    return r;

    // float oneMinusNoHSquared = 1.0 - NoH * NoH;
    // float a = NoH * roughness;
    // float k = roughness / (oneMinusNoHSquared + a * a);
    // float d = k * k * (1.0 / PI);

    // return d;
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

float F_Schlick(float f0, float f90, float VoH) 
{
    return f0 + (f90 - f0) * pow(clamp(1.0 - VoH, 0., 1.), 5.);
}

vec3 F_Schlick(vec3 f0, float VoH) 
{
    return f0 + (1. - f0) * pow(2., ( -5.55473 * VoH - 6.98316) * VoH);
    //return f0 + (vec3(1.0) - f0) * pow(1.0 - VoH, 5.0);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) 
{
    float k = (( a + 1. ) * ( a + 1. )) / 8.;
    float g1 = ( NoV ) / ( NoV * ( 1. - k ) + k );
    float g2 = ( NoL ) / ( NoL * ( 1. - k ) + k );
    return g1 * g2;

    // float a2 = a * a;
    // float GGXL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
    // float GGXV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
    // return 0.5 / (GGXV + GGXL);
}

float V_Kelemen(float LoH) //for clearcoat
{
    return 0.25 / (LoH * LoH);
}

float Fd_Lambert() 
{
    return 1.0 / PI;
}

vec3 BRDF(vec3 albedo, vec3 v, vec3 n, vec3 l, float a, float metallic) 
{
    vec3 h = normalize(v + l);
    vec3 diffuse = (1.0 - metallic) * albedo;

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);
    float VoH = clamp(dot(v, h), 0.0, 1.0);

    // perceptually linear roughness to roughness
    float roughness = a * a;
    vec3 f0 = 0.16 * roughness * (1.0 - metallic) + albedo * metallic;
    
    float D = D_GGX(roughness, NoH);
    vec3  F = F_Schlick(f0, VoH);
    float V = V_SmithGGXCorrelated(NoV, NoL, roughness);

    // specular BRDF
    vec3 Fr = ((D * V) * F) / ((4. * NoV * NoL) + 1e-5);

    vec3 Fd = diffuse * Fd_Lambert();

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;    
    kD *= 1.0 - metallic;	

    vec3 color = (kD * Fd) + Fr;

    // directional light
    float illuminance = NoL * lightIntensity1; //intensity
    color *= illuminance;

    // iBL
    vec3 kSi = fresnelSchlickRoughness(NoV, f0, roughness);
    vec3 kDi = 1.0 - kSi;
    kDi *= 1.0 - metallic;

    vec3 irradiance = texture(_IrradianceMap, n).xyz;
    vec3 ambient = (kDi * irradiance) * albedo;

    color = color + ambient;

    color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2)); 

    return irradiance;
}

void main()
{    
    vec3 albedo = texture(_Diffuse, oUv).xyz;
    vec3 position = texture(_Position, oUv).xyz;
    vec3 normal = texture(_Normal, oUv).xyz;
    vec3 roughness = texture(_Roughness, oUv).xyz;
    vec3 env = texture(_environmentMap, normal).xyz;

    //env = env / (env + vec3(1.0));
    //env = pow(env, vec3(1.0/2.2));

    vec3 viewDirClipSpace = normalize(cameraPos - position);
    
    vec3 viewDir = normalize(viewDirClipSpace).xyz;
    vec3 lightDir = normalize(lightPos1);

    vec3 c = BRDF(albedo, viewDir, normalize(normal), lightDir, roughness.x, roughness.y);

    oAccum = vec4(c,1.0);
}