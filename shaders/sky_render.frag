#version 450 core

in vec3 localPos;
  
uniform samplerCube environmentMap;

layout (location = 0) out vec4 oAccum;

void main()
{
    vec3 envColor = textureLod(environmentMap, localPos, 1.).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
  
    oAccum = vec4(envColor, 1.0);
}