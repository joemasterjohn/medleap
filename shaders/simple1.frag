#version 150

uniform float window_min;
uniform float window_multiplier;

in vec4 outcolor;
in vec2 outtexcoord;
out vec4 fColor;

uniform isampler2D texsampler;


void main()
{
    int color = texture(texsampler, outtexcoord).r;

    if (color < 0)
        fColor = vec4(1.0, 0.0, 0.0, 1.0);
    else if (color > 0)
        fColor = vec4(0.0, 1.0, 0.0, 1.0);
    else
        fColor = vec4(0.0, 0.0, 1.0, 1.0);
    
    float c = color / 32767.0;
    fColor = vec4(c, c, c, 1.0);
    
//    float c = color * 0.9;
//    
//    fColor = vec4(c, c, c, 1.0);
}