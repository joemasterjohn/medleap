#include "Color.h"
#include <algorithm>
#include "gl/math/Math.h"

using namespace std;
using namespace gl;

ColorHSV ColorRGB::hsv() const
{
	float h, s, v;

	float cmin = min(min(r, g), b);
	float cmax = max(max(r, g), b);
	float delta = cmax - cmin;
	
	v = cmax;
	if (cmax != 0.0f) {
		s = delta / cmax;
		if (cmax == r) {
			h = (g - b) / delta * pi_over_3;
			if (h < 0)
				h += two_pi;
		}
		else if (cmax == g) {
			h = ((b - r) / delta + 2.0f) * pi_over_3;
		}
		else {
			h = ((r - g) / delta + 4.0f) * pi_over_3;
		}
	} else {
		s = 0.0f;
		h = -1.0f; // undefined hue
	}

	return{ h, s, v, alpha() };
}

ColorRGB ColorHSV::rgb() const
{
	if (h < 0) {
		return{ 0.f, 0.f, 0.f };
	}

	float r, g, b;

	auto mod2 = [] (float a)->double { return a - (static_cast<int>(a) / 2) * 2; };

	float c = v * s;
	float x = c * (1.0f - abs(mod2(h / pi_over_3) - 1.0f));
	float m = v - c;

	if (h < 1) {
		return{ c + m, x + m, m, alpha() };
	}
	else if (h < 2) {
		return{ x + m, c + m, m, alpha() };
	}
	else if (h < 3) {
		return{ m, c + m, x + m, alpha() };
	}
	else if (h < 4) {
		return{ m, x + m, c + m, alpha() };
	}
	else if (h < 5) {
		return{ x + m, m, c + m, alpha() };
	}
	else {
		return{ c + m, m, x + m, alpha() };
	}
}