#include "Transfer1D.h"
#include <algorithm>
#include "util/Util.h"

using namespace gl;
using namespace std;

Transfer1D::Marker& Transfer1D::Marker::operator=(const Transfer1D::Marker& other)
{
	transfer_ = other.transfer_;
	interval_ = other.interval_;
	color_ = other.color_;
	context_ = other.context_;
	opacity_ = other.opacity_;
	return *this;
}

Transfer1D::Marker::Marker(Transfer1D* transfer, float center, float width, const ColorRGB& color, bool context) : 
	transfer_(transfer),
	interval_(center, width),
	color_(color),
	context_(context),
	opacity_(gl::exponentialC(9.f))
{
}

void Transfer1D::Marker::center(float center)
{
	interval_.center(center);
	transfer_->needs_sort_ = true;
}

float Transfer1D::Marker::opacityWeight(float x) const
{
	return opacity_(clamp(x, -1.0f, 1.0f));
}

Transfer1D::Transfer1D() : gradient_(false), needs_sort_(false)
{
}

Transfer1D::Transfer1D(const Transfer1D& other) {
	markers_ = other.markers_;
	needs_sort_ = other.needs_sort_;
	gradient_ = other.gradient_;
	for (Marker& m : markers_)
		m.transfer_ = this;
}

Transfer1D& Transfer1D::operator=(const Transfer1D& other) {
	markers_ = other.markers_;
	needs_sort_ = other.needs_sort_;
	gradient_ = other.gradient_;
	for (Marker& m : markers_)
		m.transfer_ = this;
	return *this;
}

Transfer1D::Marker& Transfer1D::add(float center, float width, const ColorRGB& color, bool context)
{
	markers_.push_back({ this, center, width, color, context });
	needs_sort_ = markers_.size() > 1;
	return markers_.back();
}

void Transfer1D::remove(float center)
{
	if (markers_.empty())
		return;
	if (markers_.size() == 1)
		markers_.clear();
	markers_.erase(find(center));
}

void Transfer1D::clear()
{
	markers_.clear();
}

Transfer1D::Marker* Transfer1D::closest(float center)
{
	if (markers_.empty())
		return nullptr;
	return &(*find(center));
}

std::vector<Transfer1D::Marker>::iterator Transfer1D::find(float center)
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
		return i.center() < value;
	});

	if (it == markers_.begin()) {
		// closest must be at position 0 or 1
		float l = it->center() - center;
		float r = (it + 1)->center() - center;
		return (l < r) ? it : it + 1;
	}

	if (it == markers_.end()) {
		// closest must be the last since value >= everything
		return it - 1;
	}

	// closest must be at i or i-1
	float l = center - (it - 1)->center();
	float r = it->center() - center;
	return (l < r) ? it - 1 : it;
}

void Transfer1D::sortMarkers()
{
	auto comp = [](Marker& i, Marker& j)->bool {
		return i.center() < j.center();
	};

	sort(markers_.begin(), markers_.end(), comp);
	needs_sort_ = false;
}


void Transfer1D::saveTexture(Texture& texture)
{
	if (markers_.empty())
		return;
	
	if (gradient_)
		saveGradient(texture);
	else
		savePiecewise(texture);
}

void Transfer1D::saveGradient(Texture& texture)
{
	if (needs_sort_)
		sortMarkers();

	static const unsigned texWidth = 512;
	unsigned short buf[texWidth * 4];
	long ptr = 0;

	auto l = markers_.begin();
	auto r = markers_.begin() + 1;

	for (int i = 0; i < texWidth; i++) {
		float p = static_cast<float>(i) / texWidth;

		if (p < markers_.front().center()) {
			Vec4 color = markers_.front().color().vec4();
			color *= {color.w, color.w, color.w, 1.0f};
			buf[ptr++] = (unsigned short)(color.x * 65535);
			buf[ptr++] = (unsigned short)(color.y * 65535);
			buf[ptr++] = (unsigned short)(color.z * 65535);
			buf[ptr++] = (unsigned short)(color.w * 65535);
		}
		else if (p > markers_.back().center()) {
			Vec4 color = markers_.back().color().vec4();
			color *= {color.w, color.w, color.w, 1.0f};
			buf[ptr++] = (unsigned short)(color.x * 65535);
			buf[ptr++] = (unsigned short)(color.y * 65535);
			buf[ptr++] = (unsigned short)(color.z * 65535);
			buf[ptr++] = (unsigned short)(color.w * 65535);
		}
		else {
			if (p > r->center()) {
				l++;
				r++;
			}

			float pn = (p - l->center()) / (r->center() - l->center());
			Vec4 lc = l->color().vec4();
			lc *= {lc.w, lc.w, lc.w, 1.0f};
			Vec4 rc = r->color().vec4();
			rc *= {rc.w, rc.w, rc.w, 1.0f};

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

void Transfer1D::savePiecewise(Texture& texture)
{
	static const unsigned texWidth = 512;
	vector<GLushort> buf;
	buf.resize(texWidth * 4, 0);

	vector<Vec4> pixels;
	pixels.resize(texWidth, Vec4(0.0f));

	for (int i = 0; i < texWidth; i++) {
		Vec4& pixel = pixels[i];
		float p = static_cast<float>(i) / texWidth;
		float sum_alpha = 0.0f;

		for (Marker& m : markers_) {
			Vec4 color = m.color().vec4();
			float x = (p - m.center()) / (m.width() * 0.5f);
			float a = color.w * m.opacityWeight(x);
			pixel += {color.x * a, color.y * a, color.z * a, a};
		}
	}

	long ptr = 0;
	for (int i = 0; i < texWidth; i++) {
		buf[ptr++] = static_cast<GLushort>(min(1.0f, pixels[i].x) * numeric_limits<GLushort>::max());
		buf[ptr++] = static_cast<GLushort>(min(1.0f, pixels[i].y) * numeric_limits<GLushort>::max());
		buf[ptr++] = static_cast<GLushort>(min(1.0f, pixels[i].z) * numeric_limits<GLushort>::max());
		buf[ptr++] = static_cast<GLushort>(min(1.0f, pixels[i].w) * numeric_limits<GLushort>::max());
	}

	texture.bind();
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	texture.setData1D(0, GL_RGBA, texWidth, GL_RGBA, GL_UNSIGNED_SHORT, &buf[0]);
}

void Transfer1D::saveContext(Texture& texture)
{
	// context texture will store 0 for context values and 255 for focus values
	static const unsigned texWidth = 512;
	vector<GLubyte> buf;
	buf.resize(texWidth, 0);

	if (gradient_) {
		// for continuous gradient, lerp pixel values between markers
		auto l = markers_.begin();
		auto r = markers_.begin() + 1;

		for (int i = 0; i < texWidth; i++) {
			float p = static_cast<float>(i) / texWidth;

			if (p < markers_.front().center()) {
				if (!markers_.front().context()) {
					buf[i] = static_cast<GLubyte>(255);
				}
			} else if (p > markers_.back().center()) {
				if (!markers_.back().context()) {
					buf[i] = static_cast<GLubyte>(255);
				}
			} else {
				if (p > r->center()) {
					l++;
					r++;
				}

				float pn = (p - l->center()) / (r->center() - l->center());
				float l_context_value = l->context() ? 0.0f : 1.0f;
				float r_context_value = r->context() ? 0.0f : 1.0f;
				float p_context_value = gl::lerp(l_context_value, r_context_value, pn);

				buf[i] = static_cast<GLubyte>(p_context_value * numeric_limits<GLubyte>::max());
			}
		}
	} else {
		// for piecewise, any marker interval that is focus will be 255 (regardless of overlapping context markers)
		for (int i = 0; i < texWidth; i++) {
			float p = static_cast<float>(i) / texWidth;
			for (Marker& m : markers_) {
				if (!m.context() && m.contains(p)) {
					buf[i] = static_cast<GLubyte>(255);
				}
			}
		}
	}

	texture.bind();
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	texture.setData1D(0, GL_R8, texWidth, GL_RED, GL_UNSIGNED_BYTE, &buf[0]);
}

float Transfer1D::center() const
{
	float l = markers_.front().center();
	float r = markers_.back().center();
	return (l + r) * 0.5f;
}

float Transfer1D::width() const
{
	float l = markers_.front().center();
	float r = markers_.back().center();
	return r - l;
}

void Transfer1D::move(const vector<float>& centers)
{
	for (int i = 0; i < markers_.size(); i++) {
		markers_[i].center(centers[i]);
	}
}