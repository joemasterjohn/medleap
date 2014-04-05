#version 150

uniform sampler3D tex_volume;
uniform sampler3D tex_gradients;
uniform sampler1D tex_clut;
uniform float window_min;
uniform float window_multiplier;
uniform vec3 volumeDimensions;
uniform bool signed_normalized;
uniform bool use_shading;
uniform bool use_isosurface;
uniform float isoValue;
uniform vec3 lightDirection;
uniform vec3 minGradient;
uniform vec3 rangeGradient;
uniform float opacity_correction;    // current sampling rate divided by reference sampling rate
uniform float opacity_scale;         // scales overall opacity

in vec3 fs_texcoord;

out vec4 display_color;

void main()
{
    // raw data value stored in red channel of texture
    float value = texture(tex_volume, fs_texcoord).r;
    
    // convert signed normalized data (ex. CT) from [-1,1] to [0,1]
    if (signed_normalized) {
        value = value * 0.5 + 0.5;
    }
    
    // discard fragment if isosurface mode
    if (use_isosurface && value < isoValue) {
        discard;
    }
    
    // map data values into value-of-interest window
    value = (value - window_min) * window_multiplier;
    value = max(min(1.0, value), 0.0);
    
    // color/opacity from look-up table using windowed data value
    vec4 color = texture(tex_clut, value).rgba;
    
    // opacity correction based on sampling rate
    color.a = 1.0 - pow(1.0 - color.a * opacity_scale, opacity_correction);

    // apply shading using stored gradients
    if (use_shading) {
        vec3 g = texture(tex_gradients, fs_texcoord).rgb;
        g.x = g.x * rangeGradient.x + minGradient.x;
        g.y = g.y * rangeGradient.y + minGradient.y;
        g.z = g.z * rangeGradient.z + minGradient.z;
        vec3 n = normalize(g);
        color.rgb *= max(min(1.0, dot(n, lightDirection)), 0.3); // .3 is ambient
    }

    display_color = color;
}