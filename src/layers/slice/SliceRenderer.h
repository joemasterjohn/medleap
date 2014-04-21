#ifndef __medleap__SliceRenderer__
#define __medleap__SliceRenderer__

#include "gl/glew.h"
#include "layers/Renderer.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "gl/Buffer.h"
#include "data/VolumeData.h"
#include "gl/math/Math.h"

/** Renders a 2D slice of the volume */
class SliceRenderer : public Renderer
{
public:
    SliceRenderer();
    void setVolume(VolumeData* volume);
    void draw();
    void resize(int width, int height);
    int getCurrentSlice();
    void setCurrentSlice(int sliceIndex);
    void setCLUTTexture(gl::Texture& texture);

private:
    VolumeData* volume;
    gl::Program sliceShader;
    gl::Texture clutTexture;
    gl::Texture sliceTexture;
    gl::Buffer sliceVBO;
    int currentSlice;
	gl::Mat4 modelMatrix;
    
    void updateTexture();
};

#endif /* defined(__medleap__SliceRenderer__) */
