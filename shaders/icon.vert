#version 330

uniform mat4 model_view;

layout (location = 0) in vec4 vs_position;
layout (location = 1) in vec2 vs_texcoord;
out vec2 fs_texcoord;

void main()
{
    gl_Position = model_view * vs_position;
    fs_texcoord = vs_texcoord;
}
