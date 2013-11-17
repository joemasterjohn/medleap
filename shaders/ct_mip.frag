#version 150

uniform float window_min;
uniform float window_multiplier;
uniform bool signed_normalized;

in vec4 outcolor;
in vec2 outtexcoord;
out vec4 fColor;

uniform sampler2D texsampler;

void main()
{
    float value = texture(texsampler, outtexcoord).r;
    if (signed_normalized) {
        // For CT data (signed values) I use signed normalized integer textures, so
        // the data must be convered from [-1,1] to [0,1]
        value = value * 0.5 + 0.5;
    }
    
    // apply VOI lut
    value = (value - window_min) * window_multiplier;
    value = max(min(1.0, value), 0.0);
    
    fColor = vec4(value, value, value, 1.0);

    
    
    // value in [-32768, 32767] because CT data is 16-bit signed
//    int value = texture(texsampler, outtexcoord).r;
//    float n = (value + 32768) / 65535.0;
//    fColor = vec4(n, n, n, 1.0);
}