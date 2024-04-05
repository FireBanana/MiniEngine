#version 450 core

precision mediump float;

layout(set=0, binding=0) uniform SceneBlock
{
    mat4 view;
    mat4 projection;
    vec3 camPos;
};

layout(location = 0) out vec3 out_color;
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec3 in_col;

void main()
{
    gl_Position = vec4(in_pos, 0., 1.0);
    out_color = in_col;
}
