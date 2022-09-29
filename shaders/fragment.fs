#version 330 core

layout (std140) uniform Matrices
{
		mat4 projection;
		mat4 view;
		mat4 lightvp;
		vec3 lightPos;
};

uniform vec3 camPos;

uniform sampler2D shadowMap;
uniform sampler2D textureMap;
uniform sampler2D normalMap;

in vec3 f_norm;
in vec3 f_pos;
in vec2 f_texcoord;
in vec4 f_posLightSpace;
in vec3 f_tangent;

out vec4 col;

float check_shadows()
{
    vec4 pos_from_light = lightvp * vec4(f_pos, 1.0);
    vec4 sample = texture(shadowMap, pos_from_light.xy);

    if(sample.z < 1.)
        return 1.;
    else
        return 0.;
}

void main()
{
    vec3 lightDir = normalize(lightPos - f_pos);

    vec3 normal = texture(normalMap, f_texcoord*0.3 ).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    // Tangent calculations
    vec3 tangent = normalize(f_tangent);
    tangent = normalize(tangent - dot(tangent, f_norm) * f_norm);
    vec3 bitangent = cross(tangent, f_norm);
    mat3 TBN = mat3(tangent, bitangent, f_norm);
    vec3 newNormal = TBN * normal;
    newNormal = normalize(newNormal);
    //col = vec4(newNormal, 1.);
    //return;
    //===========================

    //shadow calculation
    vec3 projCoords = f_posLightSpace.xyz / f_posLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z; 
    float bias = max(0.0001 * (1.0 - dot(f_norm, lightDir)), 0.0002); 
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    //==============================

    // if(projCoords.z > 1.0)
    //     shadow = 0.0;

    float mag = max(dot(lightDir, newNormal), 0.);

    vec4 shadow4 = vec4(mag + ((1-shadow) * 0.4));
    col = texture(textureMap, f_texcoord * 0.3) * shadow4;
}