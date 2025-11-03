#version 450 core 

precision mediump float;

layout(set=0, binding=0) uniform SceneBlock
{
    mat4 view;
    mat4 projection;
    vec3 camPos;
    float testUniform;
};

layout( push_constant ) uniform constant
{
    mat4 model;
} pConstant;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_col;

layout(location = 0) out vec3 out_pos;
layout(location = 1) out vec3 out_norm;
layout(location = 2) out vec2 out_col;

void main()
{
    gl_Position = projection * view * pConstant.model * vec4(in_pos, 1.0);
    out_col = in_col;
    out_pos = in_pos;
    out_norm = in_norm;
}
