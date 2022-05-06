#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texcoord;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
    vec3 lightPos;
};

out vec3 f_norm;
out vec3 f_pos;

uniform mat4 model;

void main()
{
    f_norm = mat3(transpose(inverse(model))) * norm; 
    gl_Position = projection * view * model * vec4(pos, 1.0);
    f_pos = vec3(model * vec4(pos, 1.0));
}