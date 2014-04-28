#version 330

uniform mat4 modelViewProjection;

layout (location = 0) in vec4 vs_position;

void main()
{
    gl_Position = modelViewProjection * vs_position;
}
