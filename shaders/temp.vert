#version 450 core

precision mediump float;

layout(set=0, binding=0) uniform SceneBlock
{
};

layout(location = 0) out vec3 out_color;
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec3 in_col;

vec2 triangle_positions[6] = vec2[](
    vec2(0.5, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5),
	vec2(-0.5, 0.5),
    vec2(-0.5, -0.5),
	vec2(0.5, -0.5)
);

vec3 triangle_colors[3] = vec3[](
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(1.0, 0.0, 1.0)
);

void main()
{
    gl_Position = vec4(in_pos, 0.5, 1.0);

    //out_color = triangle_colors[gl_VertexIndex % 3];
    //gl_Position = vec4(in_pos, 0.5, 1.);
    out_color = in_col;
}
