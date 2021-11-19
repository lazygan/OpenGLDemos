#version 450 core

uniform sampler2D u_tex;

in vec2 f_tex_coords;
out vec4 o_color;

void main()
{
	o_color = texture(u_tex, f_tex_coords).rgba;
}