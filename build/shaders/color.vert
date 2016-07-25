#version 150

uniform mat4 modelViewProjection;

in vec4 vs_position;
in vec4 vs_color;
out vec4 fs_color;

void main()
{
    gl_Position = modelViewProjection * vs_position;
    fs_color = vs_color;
}
