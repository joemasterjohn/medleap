#ifndef __medleap__SliceRenderer__
#define __medleap__SliceRenderer__

#include "gl/glew.h"
#include "layers/Renderer.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "gl/Buffer.h"
#include "data/VolumeData.h"
#include "math/Matrix4.h"

/** Renders a 2D slice of the volume */
class SliceRenderer : public Renderer
{
public:
    SliceRenderer();
    ~SliceRenderer();
    void init();
    void setVolume(VolumeData* volume);
    void draw();
    void resize(int width, int height);
    
    int getCurrentSlice();
    void setCurrentSlice(int sliceIndex);
    void setCLUTTexture(gl::Texture* texture);
private:
    class OrientationLabel
    {
    public:
        std::string text;
        Vec2 position;
        OrientationLabel(std::string text, Vec2 position) : text(text), position(position) {}
    };
    
    VolumeData* volume;
    gl::Program* sliceShader;
    gl::Program* axisShader;
    gl::Texture* clutTexture;
    gl::Texture* sliceTexture;
    gl::Buffer* sliceVBO;
    gl::Buffer* axisVBO;
    
    int numOrientationVertices;
    int currentSlice;
    std::vector<OrientationLabel> labels;
    Mat4 modelMatrix;
    
    void updateTexture();
    void drawSlice();
    void drawOrientationOverlay();
};

#endif /* defined(__medleap__SliceRenderer__) */
