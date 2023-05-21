#version 450 core

layout (std140, binding = 0) uniform MatrixBlock
{
	mat4 view;
	mat4 projection;
};

layout (location = 0) in vec3 iPos;

void main()
{
    gl_Position = vec4(iPos, 0.);
}