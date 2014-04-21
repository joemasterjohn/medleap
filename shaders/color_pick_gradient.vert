#version 330

uniform mat4 modelViewProjection;
uniform vec4 color1;
uniform vec4 color2;

layout (location = 0) in vec4 vs_position;
out vec4 fs_color;

void main()
{
	gl_Position = modelViewProjection * vs_position;

	float i = vs_position.y * 0.5 + 0.5;
	fs_color = color1 * i + color2 * (1.0 - i);
}
