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

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_texcoord;

layout(location = 0)out vec4 col;

void main()
{    
    ivec2 size = imageSize(_normal);
    vec2 c = in_texcoord;
    vec4 d = imageLoad(_diffuse, ivec2(c * size));

    // colorAttachment = vec4(d.x, d.y, 0, 1.);
    // positionAttachment = vec4(c.y, c.x, 0, 1.);
    // normalAttachment = vec4(c.x, c.x, 0, 1.);
    // roughnessAttachment = vec4(c.y, c.y, 0, 1.);

    // Example points
    vec3 camera = vec3(5,0,10);
    vec3 light = vec3(2,2,1);
    float lightDir = dot(light, in_norm);
    float fres = dot(normalize(camera - in_pos), normalize(in_norm));
    fres = pow(fres, 0.1);

    col = d;// * (lightDir + .3) * (fres * vec4(0.1, 0.15, 0.2, 1.)) ;
}
