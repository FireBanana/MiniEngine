#version 330 core

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
    vec3 lightPos;
};

uniform vec3 camPos;

in vec3 f_norm;
in vec3 f_pos;

out vec4 col;

void main()
{
    vec3 lightDir = normalize(lightPos - f_pos);
    float mag = max(dot(lightDir, f_norm), 0.);

    // =========

    col = vec4(vec3(mag), 1.0);
}