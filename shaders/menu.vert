#version 150

uniform mat4 modelViewProjection;

in vec4 vs_position;

void main()
{
    gl_Position = modelViewProjection * vs_position;
}
