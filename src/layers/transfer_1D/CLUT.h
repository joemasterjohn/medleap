#ifndef __MEDLEAP_TF1D_CLUT_H__
#define __MEDLEAP_TF1D_CLUT_H__

#include "gl/math/Math.h"
#include "gl/Texture.h"
#include "util/Interval.h"
#include "util/Color.h"
#include <functional>
#include <vector>

/** Color look-up table */
class CLUT
{
public:
	enum Mode
	{
		continuous,
		piecewise
	};

	class Marker
	{
	public:
		Marker(const Interval& interval, const ColorRGB& color);

		const ColorRGB& color() const { return color_; }
		const Interval& interval() const { return interval_; }
		bool context() const { return context_; }

		/** Computes opacity weight at a value x in [0,1], where 0 is the marker's left and 1 is the marker's right. Values outside 0,1 are clamped. */
		float opacity(float x) const;

		/** Sets the opacity weighting function. This function has the domain [-1,1] which maps to the marker interval's [left,right]. */
		void opacityFn(std::function<float(float)> fn) { opacity_ = fn; }

		Marker& color(const Color& color);
		Marker& interval(const Interval& interval);
		Marker& context(bool context);

	private:
		CLUT* clut_;
		Interval interval_;
		ColorRGB color_;
		bool context_;
		std::function<float(float)> opacity_;

		friend class CLUT;
	};
    
	/** Creates an empty CLUT */
    CLUT(Mode mode);

	CLUT(const CLUT& clut) {
		markers_ = clut.markers_;
		interval_ = clut.interval_;
		mode_ = clut.mode_;
		needs_sort_ = clut.needs_sort_;

		for (Marker& m : markers_)
			m.clut_ = this;
	}

	CLUT& operator=(const CLUT& clut) {
		markers_ = clut.markers_;
		interval_ = clut.interval_;
		mode_ = clut.mode_;
		needs_sort_ = clut.needs_sort_;

		for (Marker& m : markers_)
			m.clut_ = this;

		return *this;
	}

	/** Adds a new marker */
	void addMarker(const Marker& marker);

	/** Removes the closest marker to center */
	void removeMarker(float center);

	/** Removes all markers */
    void clearMarkers();
    
	/** Find marker with center closest to center */
	Marker* closestMarker(float center);

	/** Saves CLUT into a 1D texture based on mode */
	void saveTexture(gl::Texture& texture);

	Interval& interval() { return interval_; }
    
	Mode mode() const { return mode_; }

	const std::vector<Marker>& markers() const { return markers_; }

private:
	std::vector<Marker> markers_;
	Interval interval_;
	Mode mode_;
	bool needs_sort_;

	void sortMarkers();
	void saveContinuous(gl::Texture& texture);
	void savePiecewise(gl::Texture& texture);

	std::vector<Marker>::iterator find(float center);
};

#endif // __MEDLEAP_TF1D_CLUT_H__
