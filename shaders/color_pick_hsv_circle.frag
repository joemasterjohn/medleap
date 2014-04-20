#version 150

uniform float value;

in vec2 fs_position;
out vec4 display_color;

#define PI     3.14159265359
#define TWO_PI 6.28318530718
#define PI_OVER_3 1.0471975512

// hue should be in [0, 2pi)
vec4 hsv2rgb(float h, float s, float v)
{
	float H = h / PI_OVER_3;
	float c = v * s;
	float x = c * (1.0 - abs(mod(H, 2.0) - 1.0));
	float m = v - c;

	if (H < 1.0) {
		return vec4(c+m, x+m, m, 1.0);
	} else if (H < 2.0) {
		return vec4(x+m, c+m, m, 1.0);
	} else if (H < 3.0) {
		return vec4(m, c+m, x+m, 1.0);
	} else if (H < 4.0) {
		return vec4(m, x+m, c+m, 1.0);
	} else if (H < 5.0) {
		return vec4(x+m, m, c+m, 1.0);
	} else {
		return vec4(c+m, m, x+m, 1.0);
	}
}

void main()
{
	float hue = atan(fs_position.y, fs_position.x);
	if (hue < 0.0)
		hue += TWO_PI;
	float saturation = length(fs_position);

	if (saturation <= 1.0) {
		display_color = hsv2rgb(hue, saturation, value);
	} else {
		display_color = vec4(0.0);
	}
}