#ifndef __MEDLEAP_TF1D_CLUT_H__
#define __MEDLEAP_TF1D_CLUT_H__

#include "gl/math/Math.h"
#include "gl/Texture.h"
#include "util/Interval.h"
#include "util/Color.h"
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
		const ColorRGB& color() const { return color_; }
		const Interval& interval() const { return interval_; }
		bool context() const { return context_; }

		void color(const Color& color);
		void interval(const Interval& interval);
		void context(bool context);

	private:
		Marker(CLUT& clut, float center, float width);

		CLUT& clut_;
		Interval interval_;
		ColorRGB color_;
		bool context_;
		// alpha ramp?

		friend class CLUT;
	};
    
	/** Creates an empty CLUT */
    CLUT();

	/** Adds a new marker and returns a reference to it */
	Marker& addMarker(float center);

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

private:
	std::vector<std::shared_ptr<Marker>> markers_;
	Interval interval_;
	Mode mode_;
	bool needs_sort_;

	void sortMarkers();
	void saveContinuous(gl::Texture& texture);
	void savePiecewise(gl::Texture& texture);

	std::vector<std::shared_ptr<Marker>>::iterator find(float center);

	friend class Marker;
};

#endif // __MEDLEAP_TF1D_CLUT_H__
