#version 330

uniform sampler1D tex_transfer;

in float fs_texcoord;
out vec4 display_color;

void main()
{
	vec4 color = texture(tex_transfer, fs_texcoord).rgba;
    display_color = color * color.a;
}