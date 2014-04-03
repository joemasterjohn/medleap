#version 150

uniform sampler2D tex_slice;
uniform sampler1D tex_clut;
uniform float window_min;
uniform float window_multiplier;
uniform bool signed_normalized;

in vec2 fs_texcoord;
out vec4 display_color;

void main()
{
    float value = texture(tex_slice, fs_texcoord).r;
    
    // signed values need to be converted from [-1,1] to [0,1]
    if (signed_normalized)
        value = value * 0.5 + 0.5;
    
    // apply window transform
    value = (value - window_min) * window_multiplier;
    value = max(min(1.0, value), 0.0);
    
    // color/opacity from look-up table using windowed data value
    display_color = texture(tex_clut, value).rgba;
}