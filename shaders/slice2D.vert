#version 150

uniform mat4 model;

in vec4 vs_position;
in vec2 vs_texcoord;
out vec2 fs_texcoord;

void main()
{
    gl_Position = model * vs_position;
    fs_texcoord = vs_texcoord;
}
