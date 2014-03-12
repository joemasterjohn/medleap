#ifndef __medleap__SliceRenderer2D__
#define __medleap__SliceRenderer2D__

#include <GL/glew.h>
#include "render/RenderLayer.h"
#include "gl/Program.h"
#include "util/TextRenderer.h"
#include "volume/VolumeData.h"
#include "math/Matrix4.h"

/** Renders a 2D slice of the volume */
class SliceRenderer2D : public RenderLayer
{
public:
    SliceRenderer2D();
    ~SliceRenderer2D();
    void init();
    void setVolume(VolumeData* volume);
    void draw();
    void resize(int width, int height);
    
    int getCurrentSlice();
    void setCurrentSlice(int sliceIndex);
private:
    class OrientationLabel
    {
    public:
        std::string text;
        cgl::Vec2 position;
        OrientationLabel(std::string text, cgl::Vec2 position) : text(text), position(position) {}
    };
    
    VolumeData* volume;
    TextRenderer text;
    Program* sliceShader;
    Program* axisShader;
    GLuint sliceTexture;
    GLuint vao;
    GLuint vbo;
    GLuint orientationVBO;
    int numOrientationVertices;
    int windowWidth;
    int windowHeight;
    int currentSlice;
    std::vector<OrientationLabel> labels;
    cgl::Mat4 modelMatrix;
    
    void drawSlice();
    void drawOrientationOverlay();
};

#endif /* defined(__medleap__SliceRenderer2D__) */
