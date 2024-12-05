#version 450 core 

precision mediump float;

layout(set=0, binding=0) uniform SceneBlock
{
    mat4 view;
    mat4 projection;
    vec3 camPos;
};

layout (set = 1, binding = 0, rgba8) uniform image2D _diffuse;

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec3 in_col;

layout(location = 0)out vec4 col;

void main()
{
    vec2 c = in_col.xy;
    vec4 d = imageLoad(_diffuse, ivec2(c.x * 2048, c.y * 2048));

    // colorAttachment = vec4(d.x, d.y, 0, 1.);
    // positionAttachment = vec4(c.y, c.x, 0, 1.);
    // normalAttachment = vec4(c.x, c.x, 0, 1.);
    // roughnessAttachment = vec4(c.y, c.y, 0, 1.);

    col = d;
}
