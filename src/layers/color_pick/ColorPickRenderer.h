#ifndef __medleap__ColorPickRenderer__
#define __medleap__ColorPickRenderer__

#include "layers/Renderer.h"
#include "gl/geom/Rectangle.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/math/Matrix4.h"
#include "Color.h"
#include "gl/util/Draw.h"

class ColorPickRenderer : public Renderer
{
public:
	ColorPickRenderer();
	void draw() override;
	void resize(int width, int height) override;

	void choose(const ColorHSV& color) { mColor = color; }

	const gl::Rectangle<float>& circleRect() const { return mCircleRect; }
	const gl::Rectangle<float>& alphaRect() const { return mAlphaRect; }
	const gl::Rectangle<float>& valueRect() const { return mValueRect; }
	const gl::Rectangle<float>& previewRect() const { return mPreviewRect; }

private:
	gl::Buffer geomVBO;
	gl::Program circleShader;
	gl::Program selectShader;
	gl::Program gradientShader;
	gl::Rectangle<float> mCircleRect;
	gl::Rectangle<float> mAlphaRect;
	gl::Rectangle<float> mValueRect;
	gl::Rectangle<float> mPreviewRect;
	gl::Draw mCursor;
	ColorHSV mColor;
	Mat4 mProjection;

	void quad(gl::Program prog, const gl::Rectangle<float>& rect);
};

#endif // __medleap__ColorPickRenderer__
