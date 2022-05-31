#version 330 core

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
    vec3 lightPos;
    mat4 lightvp;
};

uniform vec3 camPos;
uniform sampler2D shadowMap;

in vec3 f_norm;
in vec3 f_pos;
in vec2 f_texcoord;
in vec4 f_posLightSpace;

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
    float mag = max(dot(lightDir, f_norm), 0.);

    // =========
    
    //col = vec4(check_shadows());
    
    vec3 projCoords = f_posLightSpace.xyz / f_posLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    
    //float s = step(1., texture(shadowMap, f_texcoord));

    col = vec4(pow(texture(shadowMap, f_texcoord).r, 9));
}