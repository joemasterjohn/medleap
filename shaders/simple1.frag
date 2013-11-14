#version 150
in vec4 outcolor;
in vec2 outtexcoord;
out vec4 fColor;

uniform sampler2D texsampler;

void main()
{
    vec4 tcolor = texture(texsampler, outtexcoord).rrrr;
        
   // fColor = vec4( 1.0, 0.0, 0.0, 1.0 );
        fColor =  tcolor;
}