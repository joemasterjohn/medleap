#ifndef __medleap_OrientationController__
#define __medleap_OrientationController__

#include "layers/Controller.h"
#include "util/Camera.h"
#include "data/VolumeData.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "util/TextRenderer.h"

/** Draws orientation cube in corner */
class OrientationController : public Controller
{
public:
	OrientationController();
	void camera(Camera* camera) { camera_ = camera; }
	void volume(VolumeData* volume) { volume_ = volume; }
	void draw() override;

private:
	TextRenderer text_;
	Camera* camera_;
	VolumeData* volume_;
	gl::Buffer cube_vbo_;
	gl::Buffer cube_ibo_;
	gl::Program cube_prog_;
	GLsizei face_index_count_;
	GLsizei face_index_offset_;
	GLsizei edge_index_count_;
	GLsizei edge_index_offset_;
};

#endif // __medleap_OrientationController__
