#version 150

uniform sampler2D tex_histogram;
uniform sampler2D tex_transfer;

in vec2 fs_texcoord;
out vec4 display_color;

void main()
{
    display_color = texture(tex_histogram, fs_texcoord).rrrr * 0.2 + texture(tex_transfer, fs_texcoord).rgba * 0.5;
}