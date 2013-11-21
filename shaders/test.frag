#version 150

uniform sampler3D texsampler;
uniform float window_min;
uniform float window_multiplier;
uniform bool signed_normalized;
in vec3 fs_texcoord;

out vec4 display_color;

void main()
{
    float value = texture(texsampler, fs_texcoord).r;
    
    if (signed_normalized)
        value = value * 0.5 + 0.5;
        
    value = (value - window_min) * window_multiplier;
    value = max(min(1.0, value), 0.0);
    
    display_color = vec4(value, value, value, 1.0);
//display_color = vec4(fs_texcoord, 1);
}