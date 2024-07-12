#version 450 core 

precision mediump float;

layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 colorAttachment;
layout(location = 1) out vec4 positionAttachment;
layout(location = 2) out vec4 normalAttachment;
layout(location = 3) out vec4 roughnessAttachment;

// layout(set=1, binding=0, rgba16f) uniform image2D gbuffer[4];

void main()
{
    vec3 c = in_color;

    colorAttachment = vec4(c.x, c.y, 0, 1.);
    positionAttachment = vec4(c.y, c.x, 0, 1.);
    normalAttachment = vec4(c.x, c.x, 0, 1.);
    roughnessAttachment = vec4(c.y, c.y, 0, 1.);
}
