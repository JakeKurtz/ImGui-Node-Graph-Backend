
#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texcoord;

out vec2 texcoord;

void main()
{
    texcoord = a_texcoord;
    gl_Position = vec4(a_pos, 1.0);
}
