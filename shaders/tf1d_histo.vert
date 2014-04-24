#version 330

layout (location = 0) in vec4 vs_position;
out float fs_texcoord;

void main()
{
    gl_Position = vs_position;
	fs_texcoord = vs_position.x * 0.5 + 0.5;
}
