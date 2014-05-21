#ifndef __MEDLEAP_TF1D_Transfer1D_H__
#define __MEDLEAP_TF1D_Transfer1D_H__

#include "gl/math/Math.h"
#include "gl/Texture.h"
#include "util/Interval.h"
#include "util/Color.h"
#include <functional>
#include <vector>

class Transfer1D
{
public:
	class Marker
	{
	public:
		Marker& operator=(const Marker&);
		void color(const Color& color) { color_ = color.rgb(); }
		void context(bool context) { context_ = context; }
		void center(float center);
		void width(float width) { interval_.width(width); }
		void opacityFn(std::function<float(float)> fn) { opacity_ = fn; }
		const ColorRGB& color() const { return color_; }
		bool context() const { return context_; }
		bool contains(float value) const { return interval_.contains(value); }
		float center() const { return interval_.center(); }
		float width() const { return interval_.width(); }
		float opacityWeight(float x) const;

	private:
		Marker(Transfer1D* transfer, float center, float width, const ColorRGB& color, bool context = false);
		Transfer1D* transfer_;
		Interval interval_;
		ColorRGB color_;
		bool context_;
		std::function<float(float)> opacity_;

		friend class Transfer1D;
	};
    
    Transfer1D();
	Transfer1D(const Transfer1D&);
	Transfer1D& operator=(const Transfer1D&);
	Marker& add(float center, float width, const ColorRGB& color, bool context = false);
	void remove(float center);
    void clear();
	float width() const;
	float center() const;
	bool gradient() { return gradient_; }
	void gradient(bool gradient) { gradient_ = gradient; }
	Marker* closest(float center);
	void saveTexture(gl::Texture& texture);
	void saveContext(gl::Texture& texture);    
	std::vector<Marker> const& markers() const { return markers_; }

	void move(const std::vector<float>& new_centers);

private:
	std::vector<Marker> markers_;
	bool gradient_;
	bool needs_sort_;

	void sortMarkers();
	void saveGradient(gl::Texture& texture);
	void savePiecewise(gl::Texture& texture);
	std::vector<Marker>::iterator find(float center);
};

#endif // __MEDLEAP_TF1D_Transfer1D_H__
