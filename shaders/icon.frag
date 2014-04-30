#version 330

uniform sampler2D tex_input;
uniform vec4 color;
in vec2 fs_texcoord;
out vec4 display_color;

void main()
{
    display_color = texture(tex_input, fs_texcoord) * color;
}