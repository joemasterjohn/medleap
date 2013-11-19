#version 150

uniform mat4 model;

in vec4 vs_position;
in vec4 vs_color;
out vec4 fs_color;

void main()
{
    gl_Position = model * vs_position;
    fs_color = vs_color;
}
