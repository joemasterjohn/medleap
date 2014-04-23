#include "CLUT.h"
#include <algorithm>

using namespace gl;
using namespace std;

CLUT::Marker::Marker(const Interval& interval, const ColorRGB& color) : 
	clut_(nullptr),
	interval_(interval),
	color_(color),
	context_(false)
{
}

CLUT::Marker& CLUT::Marker::color(const Color& color)
{
	color_ = color.rgb();
	return *this;
}

CLUT::Marker& CLUT::Marker::interval(const Interval& interval)
{
	interval_ = interval;
	if (clut_) {
		clut_->needs_sort_ = true;
	}
	return *this;
}

CLUT::Marker& CLUT::Marker::context(bool context)
{
	context_ = true;
	return *this;
}




CLUT::CLUT() : mode_(continuous), needs_sort_(false)
{
}

void CLUT::addMarker(const Marker& marker)
{
	Marker copy = marker;
	copy.clut_ = this;
	markers_.push_back(copy);
	needs_sort_ = markers_.size() > 1;
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

	return &(*find(value));
}

std::vector<CLUT::Marker>::iterator CLUT::find(float center)
{
	if (markers_.empty())
		return markers_.end();

	if (markers_.size() == 1)
		return markers_.begin();

	if (needs_sort_)
		sortMarkers();

	// find iterator to first marker with interval center >= value
	auto it = lower_bound(markers_.begin(), markers_.end(), center,
		[](const Marker& i, float value)->bool{
		return i.interval().center() < value;
	});

	if (it == markers_.begin()) {
		// closest must be at position 0 or 1
		float l = it->interval().center() - center;
		float r = (it + 1)->interval().center() - center;
		return (l < r) ? it : it + 1;
	}

	if (it == markers_.end()) {
		// closest must be the last since value >= everything
		return it - 1;
	}

	// closest must be at i or i-1
	float l = center - (it - 1)->interval().center();
	float r = it->interval().center() - center;
	return (l < r) ? it - 1 : it;
}

void CLUT::sortMarkers()
{
	auto comp = [](Marker& i, Marker& j)->bool {
		return i.interval().center() < j.interval().center();
	};

	sort(markers_.begin(), markers_.end(), comp);
	needs_sort_ = false;
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
	static const unsigned texWidth = 512;
	unsigned short buf[texWidth * 4];
	long ptr = 0;

	auto l = markers_.begin();
	auto r = markers_.begin() + 1;

	for (int i = 0; i < texWidth; i++) {
		float p_tex = static_cast<float>(i) / texWidth;
		float p_clut = (p_tex - interval_.left()) / interval_.width();

		if (p_clut < 0.0f) {
			Vec4 color = markers_.front().color().vec4();
			buf[ptr++] = (unsigned short)(color.x * 65535);
			buf[ptr++] = (unsigned short)(color.y * 65535);
			buf[ptr++] = (unsigned short)(color.z * 65535);
			buf[ptr++] = (unsigned short)(color.w * 65535);
		}
		else if (p_clut > 1.0f) {
			Vec4 color = markers_.back().color().vec4();
			buf[ptr++] = (unsigned short)(color.x * 65535);
			buf[ptr++] = (unsigned short)(color.y * 65535);
			buf[ptr++] = (unsigned short)(color.z * 65535);
			buf[ptr++] = (unsigned short)(color.w * 65535);
		}
		else {
			if (p_clut > r->interval().center()) {
				l++;
				r++;
			}

			float pn = (p_clut - l->interval().center()) / (r->interval().center() - l->interval().center());
			Vec4 lc = l->color().vec4();
			Vec4 rc = r->color().vec4();

			Vec4 color = lc * (1.0f - pn) + rc * pn;
			buf[ptr++] = (unsigned short)(color.x * 65535);
			buf[ptr++] = (unsigned short)(color.y * 65535);
			buf[ptr++] = (unsigned short)(color.z * 65535);
			buf[ptr++] = (unsigned short)(color.w * 65535);
		}
	}

	texture.bind();
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	texture.setData1D(0, GL_RGBA, texWidth, GL_RGBA, GL_UNSIGNED_SHORT, buf);
}

void CLUT::savePiecewise(Texture& texture)
{
	static const unsigned texWidth = 512;
	vector<GLushort> buf;
	buf.resize(texWidth * 4, 0);

	for (Marker& m : markers_) {
		long ptr = 0;

		for (int i = 0; i < texWidth; i++) {
			float p = static_cast<float>(i) / texWidth;

			if (p >= m.interval().left() && p <= m.interval().right()) {

				float s = clamp(1.0f - abs(m.interval().center() - p) / m.interval().width() * 2.0f, 0.0f, 1.0f);
				Vec4 color = m.color().vec4();
				color.w *= s;

				long o = (i * 4);
				buf[o] += (unsigned short)(color.x * 65535);
				buf[o+1] += (unsigned short)(color.y * 65535);
				buf[o+2] += (unsigned short)(color.z * 65535);
				buf[o+3] += (unsigned short)(color.w * 65535);
			}
		}
	}

	texture.bind();
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	texture.setData1D(0, GL_RGBA, texWidth, GL_RGBA, GL_UNSIGNED_SHORT, &buf[0]);
}