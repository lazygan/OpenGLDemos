#version 450 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_color;
layout (location = 3) in vec2 v_tex_coord;

uniform mat4 M;
uniform mat4 VP;

out vec3 f_world_pos;
out vec3 f_normal;

void main()
{
    f_world_pos = vec3(M * vec4(v_position, 1.0f));
    f_normal = v_normal;
    gl_Position = VP * vec4(f_world_pos, 1.0f);
}


