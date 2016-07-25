#version 150

uniform float offset;

in vec4 vs_position;

void main()
{
    gl_Position = vs_position + vec4(offset, 0.0, 0.0, 0.0);
}
