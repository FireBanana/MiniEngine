#version 450 core

layout(location = 1) out vec4 positionAttachment;
layout(location = 2) out vec4 normalAttachment;
layout(location = 3) out vec4 roughnessAttachment;
layout(location = 4) out vec4 swapchainAttachment;

void main()
{
    swapchainAttachment = vec4(1.);
}
