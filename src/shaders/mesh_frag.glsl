#version 450 core

in vec3 f_world_pos;
in vec3 f_normal;
out vec4 color;

void main()
{
    color.rgb = f_world_pos;
    color.a = 1.0f;
}