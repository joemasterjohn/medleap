#ifndef __medleap_ListRenderer__
#define __medleap_ListRenderer__

#include "layers/Controller.h"
#include "DirectoryMenu.h"
#include "gl/Program.h"
#include "gl/Buffer.h"
#include "util/TextRenderer.h"

class ListRenderer
{
public:
	ListRenderer();
	void draw();
	void update(const DirectoryMenu& menu, const gl::Viewport& viewport);
	void model(const gl::Mat4& model) { model_ = model; }

	void alpha(float alpha) { alpha_ = alpha; }
	void highlight(int index) { highlighted_ = index; }
	float contentHeight() const { return content_height_; }
	float itemHeight() const { return item_height_; }
	float verticalPad() const { return vertical_pad_; }
	float horizontalPad() const { return horizontal_pad_; }

private:
	gl::Mat4 projection_;
	gl::Mat4 model_;
	gl::Program prog_;
	gl::Buffer vbo_;
	gl::Buffer ibo_;
	float alpha_;
	float horizontal_pad_;
	float vertical_pad_;
	float item_height_;
	float content_height_;
	GLsizei index_count_;
	GLsizei indices_per_item_;
	TextRenderer text_;
	int highlighted_;

	void drawBoxes();
};

#endif // __medleap_ListRenderer__
