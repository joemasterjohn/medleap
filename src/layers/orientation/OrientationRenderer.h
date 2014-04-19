#ifndef __medleap__OrientationRenderer__
#define __medleap__OrientationRenderer__

#include "layers/Renderer.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/math/Matrix4.h"
#include "util/Camera.h"
#include "data/VolumeData.h"

class OrientationRenderer : public Renderer
{
public:
	OrientationRenderer();
    void draw() override;    
    void resize(int width, int height) override;
	void setCamera(Camera* camera) { this->camera = camera; }
	void setVolume(VolumeData* volume) { this->volume = volume; }
private:
	gl::Buffer geomVBO;
	gl::Buffer geomIBO;
	gl::Program geomShader;
	GLsizei faceIndicesCount;
	GLsizei faceIndicesOffset;
	GLsizei edgeIndicesCount;
	GLsizei edgeIndicesOffset;
	Mat4 modelViewProj;
	Camera* camera;
	VolumeData* volume;
};

#endif // __medleap__Orientation_Renderer__
