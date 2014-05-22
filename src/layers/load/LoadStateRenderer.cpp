#include "LoadStateRenderer.h"
#include "util/stb_image.h"
#include "gl/geom/SegmentRing.h"

using namespace gl;
using namespace std;
using namespace std::chrono;

LoadStateRenderer::LoadStateRenderer() : 
	index_count_(0),
	angle_(0.0f),
	angle_step_(20.0f)
{
	prog_ = Program::create("shaders/menu.vert", "shaders/menu.frag");
	text_.loadFont("menlo18");
	initVBO();
}

void LoadStateRenderer::initVBO()
{
	int segments = 8;
	Geometry g = SegmentRing(segments, 48.0f, 64.0f).triangles();

	vbo_.generateVBO(GL_STATIC_DRAW);
	vbo_.bind();
	vbo_.data(&g.vertices[0], g.vertices.size() * sizeof(Vec3));

	ibo_.generateIBO(GL_STATIC_DRAW);
	ibo_.bind();
	ibo_.data(&g.indices[0], g.indices.size() * sizeof(GLuint));
	indices_per_segment_ = g.indices.size() / segments;
	index_count_ = g.indices.size();
}

void LoadStateRenderer::update(const VolumeLoader& loader, milliseconds elapsed, const Viewport& viewport)
{
	angle_ += angle_step_;

	Mat4 m_trans = translation(viewport.width / 2.0f, viewport.height / 2.0f, 0.0f);
	Mat4 m_rot = rotationZ(deg_to_rad * static_cast<int>(angle_ / 45.0f) * 45.0f);;
	transform_ = viewport.orthoProjection() * m_trans * m_rot;

	text_.viewport(viewport);
	text_.clear();
	text_.hAlign(TextRenderer::HAlign::center);
	text_.vAlign(TextRenderer::VAlign::center);
	text_.color(0.0f, 0.0f, 0.0f, 1.0f);
	text_.add(loader.getStateMessage(), viewport.width / 2.0f - 1, viewport.height / 2.0f - 101);
	text_.color(1.0f, 1.0f, 1.0f, 1.0f);
	text_.add(loader.getStateMessage(), viewport.width/2.0f, viewport.height/2.0f - 100);
}

void LoadStateRenderer::draw()
{
	prog_.enable();
	prog_.uniform("modelViewProjection", transform_);
	prog_.uniform("color", 0.5f, 0.5f, 0.5f, 1.0f);
	vbo_.bind();
	ibo_.bind();

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_SUBTRACT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, (GLvoid*)(indices_per_segment_ * sizeof(GLuint)));
	glBlendEquation(GL_FUNC_ADD);


	text_.draw();
}