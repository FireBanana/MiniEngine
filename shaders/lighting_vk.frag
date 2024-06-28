#version 450 core
layout(location = 4) out vec4 swapchainAttachment;
layout (set = 0, binding = 0, rgba8) uniform image2D color;

// These attachments not needed

void main()
{
    swapchainAttachment = imageLoad(color, ivec2(gl_FragCoord.x, gl_FragCoord.y));
}
