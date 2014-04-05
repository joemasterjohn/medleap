#version 150

/*
 * This shader is used for drawing the color look-up table as a horizontal bar. The CLUT
 * drawn inside the region affected by the 1D transfer function window. Pixels to the
 * left are clamped to the start color, and pixels to the right clamped to the end color.
 */

uniform vec2 x_offsets;

in vec4 vs_position;
in float vs_texcoord;
out float fs_xcoord;
out float fs_texcoord;

void main()
{
    // x coordinate will change based on where the window is
    // rather than recreated the vertices, the vertices store a mask for the uniform offsets
    float x = vs_position.x + dot(vs_position.zw, x_offsets);
    float y = vs_position.y;
    gl_Position = vec4(x, y, 0.0, 1.0);
    
    fs_xcoord = x;
    fs_texcoord = vs_texcoord;
}
