#version 330

uniform float interval_left;
uniform float interval_reciprocal_width;
uniform bool continuous_mode;

layout (location = 0) in vec4 vs_position;
out float fs_texcoord;

void main()
{
    gl_Position = vs_position;
	fs_texcoord = vs_position.x * 0.5 + 0.5;

	if (continuous_mode) {
		fs_texcoord = (fs_texcoord - interval_left) * interval_reciprocal_width;
	}
}
