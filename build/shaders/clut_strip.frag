#version 150

uniform sampler1D tex_input;
in float fs_texcoord;
in float fs_xcoord;
out vec4 display_color;

void main()
{
    vec4 stripe_color = vec4(min(max(0.05, float(int(100 * fs_xcoord) % 3)), 0.1));
    vec4 clut_color = texture(tex_input, fs_texcoord);
    display_color = vec4(clut_color.rgb * clut_color.a + stripe_color.rgb * (1.0 - clut_color.a), 1.0);
}