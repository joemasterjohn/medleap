#version 330

uniform sampler1D tex_transfer;

in float fs_texcoord;
out vec4 display_color;

void main()
{
	vec4 color = texture(tex_transfer, fs_texcoord).rgba;
	vec4 pattern = vec4(float(int(mod(fs_texcoord*160, 2.0)))) * 0.1;
    display_color = color * color.a + pattern * (1.0 - color.a);
}