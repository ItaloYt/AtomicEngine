#version 450 core

layout (location = 0) in vec2 vertex;

layout (binding = 0) uniform Block {
    mat4 projection;
    mat4 view;
} block;

void main()
{
    gl_Position = block.projection * block.view * vec4(vertex, 0, 1);
}
