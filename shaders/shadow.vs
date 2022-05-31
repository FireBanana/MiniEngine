#version 330 core

uniform mat4 viewprojection;
uniform mat4 model;

void main()
{
    gl_Position = viewprojection * model * vec4(0.0, 0.0, 0.0, 1.0);
}