#include "CLUT.h"
#include <algorithm>

using namespace gl;
using namespace std;

typedef std::shared_ptr<CLUT::Marker> MarkerPtr;

CLUT::Marker::Marker(CLUT& clut, float center, float width) : 
	clut_(clut),
	interval_(center, width),
	color_(1.0f, 1.0f, 1.0f, 1.0f),
	context_(false)
{
}

void CLUT::Marker::color(const Color& color)
{
	color_ = color.rgb();
}

void CLUT::Marker::interval(const Interval& interval)
{
	interval_ = interval;
	clut_.needs_sort_ = true;
}

void CLUT::Marker::context(bool context)
{
	context_ = true;
}




CLUT::CLUT() : mode_(continuous), needs_sort_(true)
{
}

CLUT::Marker& CLUT::addMarker(float center)
{
	MarkerPtr p(new Marker({ *this, center, 0.2f }));
	markers_.push_back(p);

	if (markers_.size() == 1) {
		return *(markers_.front().get());
	}

	sortMarkers();
	return *(p.get());
}

void CLUT::removeMarker(float center)
{
	if (markers_.empty())
		return;

	if (markers_.size() == 1)
		markers_.clear();

	markers_.erase(find(center));
}

void CLUT::clearMarkers()
{
	markers_.clear();
}

CLUT::Marker* CLUT::closestMarker(float value)
{
	if (markers_.empty())
		return nullptr;

	return find(value)->get();
}

std::vector<MarkerPtr>::iterator CLUT::find(float center)
{
	if (markers_.empty())
		return markers_.end();

	if (markers_.size() == 1)
		return markers_.begin();

	if (needs_sort_)
		sortMarkers();

	// find iterator to first marker with interval center >= value
	auto it = lower_bound(markers_.begin(), markers_.end(), center,
		[](const MarkerPtr& i, float value)->bool{
		return i.get()->interval().center() < value;
	});

	if (it == markers_.begin()) {
		// closest must be at position 0 or 1
		float l = it->get()->interval().center() - center;
		float r = (it + 1)->get()->interval().center() - center;
		return (l < r) ? it : it + 1;
	}

	if (it == markers_.end()) {
		// closest must be the last since value >= everything
		return it - 1;
	}

	// closest must be at i or i-1
	float l = center - (it - 1)->get()->interval().center();
	float r = it->get()->interval().center() - center;
	return (l < r) ? it - 1 : it;
}

void CLUT::sortMarkers()
{
	auto comp = [](MarkerPtr& i, MarkerPtr& j)->bool
	{
		return i.get()->interval().center() < j.get()->interval().center();
	};

	sort(markers_.begin(), markers_.end(), comp);

	needs_sort_ = false;
}


Vec4 CLUT::color(float position)
{
    // make sure position is in [0,1]
    position = std::min(std::max(position, 0.0f), 1.0f);
    
	return Vec4(1.0f);

    // find the nearest color stops on the left and right of position
    //ColorStop* left = &stops[findLeftStop(position)];
    //ColorStop* right = &stops[findRightStop(position)];
    //
    // left and right will be the same color stop if position is exactly the same as the stop's position
    //if (left == right)
    //    return left->getColor();
    //
    // calculate the position normalized w.r.t. left and right color stops
    //float lp = left->getPosition();
    //float rp = right->getPosition();
    //float np = (position - lp) / (rp - lp);
    //
    // return linearly interpolated color
    //return left->getColor() * (1.0f - np) + right->getColor() * np;
}


void CLUT::saveTexture(Texture& texture)
{
	if (markers_.empty())
		return;

	if (mode_ == continuous)
		saveContinuous(texture);
	else
		savePiecewise(texture);
}

void CLUT::saveContinuous(Texture& texture)
{
	// why 256? make this variable or const somewhere
	const unsigned texWidth = 256;
	unsigned short buf[texWidth * 4];
	long ptr = 0;

	auto l = markers_.begin();
	auto r = markers_.begin() + 1;

	for (int i = 0; i < texWidth; i++) {
		float p = static_cast<float>(i) / texWidth;
		if (p > r->get()->interval().center()) {
			l++;
			r++;
		}

		float pn = (p - l->get()->interval().center()) / (r->get()->interval().center() - l->get()->interval().center());
		Vec4 lc = l->get()->color().vec4();
		Vec4 rc = r->get()->color().vec4();

		// pre multiply alpha
		//lc *= Vec4(lc.w, lc.w, lc.w, 1);
		//rc *= Vec4(rc.w, rc.w, rc.w, 1);

		Vec4 color = lc * (1.0f - pn) + rc * pn;
		buf[ptr++] = (unsigned short)(color.x * 65535);
		buf[ptr++] = (unsigned short)(color.y * 65535);
		buf[ptr++] = (unsigned short)(color.z * 65535);
		buf[ptr++] = (unsigned short)(color.w * 65535);
	}

	texture.bind();
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	texture.setData1D(0, GL_RGBA, texWidth, GL_RGBA, GL_UNSIGNED_SHORT, buf);
}

void CLUT::savePiecewise(Texture& texture)
{

}