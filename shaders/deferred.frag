#version 450 core 

precision mediump float;

layout(set=0, binding=0) uniform SceneBlock
{
    mat4 view;
    mat4 projection;
    vec3 camPos;
};

layout (set = 1, binding = 0, rgba8) uniform image2D _diffuse;
layout (set = 1, binding = 1, rgba8) uniform image2D _normal;
layout (set = 1, binding = 2, rgba8) uniform image2D _roughness;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

layout(location = 0) out vec4 colorAttachment;
layout(location = 1) out vec4 positionAttachment;
layout(location = 2) out vec4 normalAttachment;
layout(location = 3) out vec4 roughnessAttachment;

void main()
{
    vec3 c = in_position;

    colorAttachment = vec4(c.x, c.y, 0, 1.);
    positionAttachment = vec4(c.y, c.x, 0, 1.);
    normalAttachment = vec4(c.x, c.x, 0, 1.);
    roughnessAttachment = vec4(c.y, c.y, 0, 1.);
}
