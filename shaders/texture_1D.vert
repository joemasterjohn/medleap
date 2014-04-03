#version 150

in vec4 vs_position;
in float vs_texcoord;
out float fs_texcoord;

void main()
{
    gl_Position = vs_position;
    fs_texcoord = vs_texcoord;
}
