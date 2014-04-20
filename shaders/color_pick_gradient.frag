#version 150

in vec4 fs_color;
out vec4 display_color;

void main()
{
	display_color = fs_color;
}