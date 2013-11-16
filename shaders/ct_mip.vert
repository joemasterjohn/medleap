#version 150

in vec4 vPosition;
in vec2 vTexCoord;

out vec4 outcolor;
out vec2 outtexcoord;

void main()
{
    gl_Position = vPosition;
    outcolor = vec4(vTexCoord,1,1);
    outtexcoord = vTexCoord;
}
