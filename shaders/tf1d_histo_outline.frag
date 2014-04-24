#version 330

uniform vec4 color;

out vec4 display_color;

void main()
{
    display_color = color;
}