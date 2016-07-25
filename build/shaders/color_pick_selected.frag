#version 330

uniform vec4 color;

in vec2 fs_position;
out vec4 display_color;

void main()
{
	vec4 bgColor = (fs_position.y < fs_position.x) ? vec4(0.0) : vec4(1.0);
	display_color = bgColor * (1 - color.a) + color * color.a;
}