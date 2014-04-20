#version 150

uniform vec4 color;
uniform vec2 tiles;

in vec2 fs_position;
out vec4 display_color;

void main()
{
	int checker_x = int(mod((fs_position.x*0.5+0.5)*tiles.x, 2.0));
	int checker_y = int(mod((fs_position.y*0.5+0.5)*tiles.y, 2.0));
	int checkered = checker_x ^ checker_y;
	display_color = vec4(checkered) * (1 - color.a) + color * color.a;
}