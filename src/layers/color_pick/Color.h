#ifndef medleap_Color_h
#define medleap_Color_h

#include "gl/math/Math.h"

class ColorRGB;
class ColorHSV;

/** A color with alpha (transparency) */
class Color
{
public:
	/** Creates a new color with alpha */
	Color(float alpha) : a(alpha) {}

	/** The alpha or opacity value in [0,1] */
	float alpha() const { return a; }

	/** Set alpha in [0,1] */
	Color& alpha(float alpha) { a = alpha; return *this; }

	/** A copy of the color in RGB color space */
	virtual ColorRGB rgb() const = 0;

	/** A copy of the color in HSV color space */
	virtual ColorHSV hsv() const = 0;

private:
	float a;
};


/** A color in RGB space */
class ColorRGB : public Color
{
public:
	ColorRGB(float r, float g, float b) : Color(1.0f), r(r), g(g), b(b) {}
	ColorRGB(float r, float g, float b, float a) : Color(a), r(r), g(g), b(b) {}
	ColorRGB(const ColorRGB& c) : Color(c.alpha()), r(c.r), g(c.g), b(c.b) {}

	/** The red component in [0,1] */
	float red() { return r; }

	/** The green component in [0,1] */
	float green() { return g; }

	/** The blue component in [0,1] */
	float blue() { return b; }

	/** The red, green, and blue components in [0,1] */
	gl::Vec3 vec3() const { return{ r, g, b }; }

	/** The red, green, blue, and alpha components in [0,1] */
	gl::Vec4 vec4() const { return{ r, g, b, alpha() }; }

	/** Set red in [0,1] */
	ColorRGB& red(float red) { r = red; return *this;  }

	/** Set green in [0,1] */
	ColorRGB& green(float green) { g = green; return *this; }

	/** Set blue in [0,1] */
	ColorRGB& blue(float blue) { b = blue; return *this; }

	ColorRGB rgb() const override { return{ r, g, b, alpha() }; }
	ColorHSV hsv() const override;

private:
	float r, g, b;
};


/** A color in HSV space */
class ColorHSV : public Color
{
public:
	ColorHSV(float h, float s, float v) : Color(1.0f), h(h), s(s), v(v) {}
	ColorHSV(float h, float s, float v, float a) : Color(a), h(h), s(s), v(v) {}
	ColorHSV(const ColorHSV& c) : Color(c.alpha()), h(c.h), s(c.s), v(c.v) {}

	/** The hue in [0,2pi). A negative value means the hue is undefined. */
	float hue() const { return h; }

	/** The saturation in [0,1] */
	float saturation() const { return s; }

	/** The value (brightness) in [0,1] */
	float value() const { return v; }

	/** Set hue in [0,2pi) */
	ColorHSV& hue(float hue) { h = hue; return *this; }

	/** Set saturation in [0,1] */
	ColorHSV& saturation(float saturation) { s = saturation; return *this; }
	
	/** Set value (brightness) in [0,1] */
	ColorHSV& value(float value) { v = value; return *this; }

	ColorRGB rgb() const override;
	ColorHSV hsv() const override { return{ h, s, v, alpha() }; }

private:
	float h, s, v;
};


#endif