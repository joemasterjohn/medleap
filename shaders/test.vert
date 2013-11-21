#version 150

uniform vec3 volumeMin;
uniform vec3 volumeDimensions;

uniform mat4 modelViewProjection;

in vec4 vs_position;

out vec3 fs_texcoord;

void main()
{
    fs_texcoord = (vs_position.xyz - volumeMin) / volumeDimensions;

    gl_Position = modelViewProjection * vs_position;
}
