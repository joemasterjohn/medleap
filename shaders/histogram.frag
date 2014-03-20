#version 150

uniform sampler2D texsampler;

in vec2 fs_texcoord;
out vec4 display_color;

void main()
{
    display_color = texture(texsampler, fs_texcoord).rrrr * 0.2;
}