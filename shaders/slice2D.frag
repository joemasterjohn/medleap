#version 150

uniform sampler2D texsampler;
uniform float window_min;
uniform float window_multiplier;
uniform bool signed_normalized;

in vec2 fs_texcoord;
out vec4 display_color;

void main()
{
    float value = texture(texsampler, fs_texcoord).r;
    
    // signed values need to be converted from [-1,1] to [0,1]
    if (signed_normalized)
        value = value * 0.5 + 0.5;
    
    // apply window transform
    value = (value - window_min) * window_multiplier;
    value = max(min(1.0, value), 0.0);
    
    display_color = vec4(value, value, value, 1.0);
}