#ifndef __medleap_LoadStateRenderer__
#define __medleap_LoadStateRenderer__

#include "layers/Controller.h"
#include "gl/Program.h"
#include "gl/Buffer.h"
#include "util/TextRenderer.h"
#include "data/VolumeLoader.h"

class LoadStateRenderer
{
public:
	LoadStateRenderer();
	void draw();
	void update(const VolumeLoader& loader, std::chrono::milliseconds elapsed, const gl::Viewport& viewport);

private:
	gl::Mat4 transform_;
	TextRenderer text_;
	gl::Program prog_;
	gl::Buffer vbo_;
	gl::Buffer ibo_;
	GLsizei index_count_;
	GLsizei indices_per_segment_;
	float angle_;
	float angle_step_;

	void initVBO();
};

#endif // __medleap_LoadStateRenderer__
