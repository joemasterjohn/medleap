#version 150

// Volume Vertex Shader - 1D CLUT
// For volume rendering or max intensity projection using a window and 1-dimensinoal color look-up table

uniform vec3 volumeMin;
uniform vec3 volumeDimensions;
uniform mat4 modelViewProjection;
uniform mat4 modelView;

in vec4 vs_position;
out vec3 fs_texcoord;
out vec3 fs_voxel_position_ws; // world space
out vec3 fs_voxel_position_es; // eye space
out vec3 fs_voxel_position_ss; // screen space

void main()
{
    fs_texcoord = (vs_position.xyz - volumeMin) / volumeDimensions;
    gl_Position = modelViewProjection * vs_position;
	fs_voxel_position_ss = gl_Position.xyz / gl_Position.w;
	fs_voxel_position_es = (modelView * vs_position).xyz;
	fs_voxel_position_ws = vs_position.xyz;
}
