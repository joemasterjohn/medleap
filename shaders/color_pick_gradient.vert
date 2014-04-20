#version 150

uniform mat4 modelViewProjection;

in vec4 vs_position;
out vec4 fs_color;

void main()
{
	gl_Position = modelViewProjection * vs_position;
	fs_color = vec4(vs_position.y * 0.5 + 0.5);
}
