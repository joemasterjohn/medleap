#version 150

uniform sampler1D tex_input;
in float fs_texcoord;
out vec4 display_color;

void main()
{
    display_color = texture(tex_input, fs_texcoord);
}