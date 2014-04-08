#version 150

uniform sampler3D tex_volume;
uniform sampler3D tex_gradients;
uniform sampler1D tex_clut;
uniform float window_min;
uniform float window_multiplier;
uniform vec3 volumeDimensions;
uniform bool signed_normalized;
uniform bool use_shading;
uniform float isoValue;
uniform vec3 lightDirection;
uniform vec3 minGradient;
uniform vec3 rangeGradient;
uniform float opacity_correction;    // current sampling rate divided by reference sampling rate
uniform float opacity_scale;         // scales overall opacity
uniform int render_mode;

#define RENDER_MODE_MIP 0
#define RENDER_MODE_VR 1
#define RENDER_MODE_ISO 2
#define AMBIENT 0.3

in vec3 fs_texcoord;
out vec4 display_color;

float shading()
{
	// get gradient vector and scale it back to original
	vec3 g = texture(tex_gradients, fs_texcoord).rgb;
	g.x = g.x * rangeGradient.x + minGradient.x;
	g.y = g.y * rangeGradient.y + minGradient.y;
	g.z = g.z * rangeGradient.z + minGradient.z;

	// use normalized gradient as lighting normal
	vec3 n = normalize(g);
	return max(min(1.0, dot(n, lightDirection)), AMBIENT);
}

void main()
{
    // get raw value stored in volume (normalized to [0, 1])
    float value = texture(tex_volume, fs_texcoord).r;
    if (signed_normalized) {
        value = value * 0.5 + 0.5;
    }

	// apply value-of-interest (window) LUT
    value = (value - window_min) * window_multiplier;
    value = max(min(1.0, value), 0.0);
    
	// color/opacity from look-up table using windowed data value
	vec4 color = texture(tex_clut, value).rgba;


	if (render_mode == RENDER_MODE_MIP) {

	} else if (render_mode == RENDER_MODE_VR) {
		// opacity correction
		float alpha_stored = color.a;
		float alpha_corrected = 1.0 - pow(1.0 - alpha_stored * opacity_scale, opacity_correction);
		color.a = alpha_corrected;

		// Pre-multiplied alpha, but I have to compensate for change in alpha
		//color.rgb *= alpha_corrected / alpha_stored;

		//color.rgb *= color.a;

		// apply lighting
		if (use_shading) {
			color.rgb *= shading();
		}
	} else if (render_mode == RENDER_MODE_ISO) {
		if (value < isoValue) {
			discard;
		}

		color.rgb = vec3(1.0);

		// apply lighting
		if (use_shading) {
			color.rgb *= shading();
		}
	}

    display_color = color;
}