#version 450 core

precision mediump float;

layout(location = 0) in vec3 in_color;

layout(set=1, binding=0, rgba16f) uniform image2D someImage;
layout(set=1, binding=1, rgba16f) uniform image2D someImage2;
layout(set=1, binding=2, rgba16f) uniform image2D someImage3;
layout(set=1, binding=3, rgba16f) uniform image2D someImage4;

void main()
{
    imageStore(someImage, ivec2(gl_FragCoord.x, gl_FragCoord.y), ivec4(1.));
    //out_color = vec4(in_color, 1.0);
}
