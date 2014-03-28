#version 150

uniform sampler3D tex_volume;
uniform sampler3D tex_gradients;
uniform float window_min;
uniform float window_multiplier;
uniform vec3 volumeDimensions;
uniform bool signed_normalized;
uniform vec3 lightDirection;
uniform vec3 minGradient;
uniform vec3 rangeGradient;
uniform float opacityCorrection;    // current sampling rate divided by reference sampling rate

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
    
    // map data values into value-of-interest window
    value = (value - window_min) * window_multiplier;
    value = max(min(1.0, value), 0.0);
    
    // apply CLUT
    
    // TODO
    // TODO: temp remove (should come from CLUT texture)
    float alpha = value * value; // linear alpha (could also be log, or tent?)

    // opacity correction based on sampling rate
    alpha = 1.0 - pow(1.0 - alpha, opacityCorrection);

    // transform gradient texture color to original gradient for shading (0,1 conversion to -1,1)
    vec3 g = texture(tex_gradients, fs_texcoord).rgb;
    g.x = g.x * rangeGradient.x + minGradient.x;
    g.y = g.y * rangeGradient.y + minGradient.y;
    g.z = g.z * rangeGradient.z + minGradient.z;
    vec3 n = normalize(g);
    float shading = max(min(1.0, dot(n, lightDirection)), 0.25);

    vec3 rgb = (vec3(1.0, 0.0, 0.0) * (1.0 - value) + value * vec3(1.0)) * shading;
    display_color = vec4(rgb, alpha);
    
//    display_color = vec4(g, 0.02);
}