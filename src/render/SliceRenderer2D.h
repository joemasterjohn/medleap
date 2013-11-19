#ifndef __medleap__SliceRenderer2D__
#define __medleap__SliceRenderer2D__

#include <GL/glew.h>
#include "gl/Program.h"
#include "gl/TextRenderer.h"
#include "volume/DCMImageSeries.h"
#include "math/Math.h"

class SliceRenderer2D
{
public:
    SliceRenderer2D();
    ~SliceRenderer2D();
    void init();
    void setVolume(DCMImageSeries* volume);
    void draw();
    void resize(int width, int height);
    
    int getCurrentSlice();
    void setCurrentSlice(int sliceIndex);
    
    int getCurrentWindow();
    void setCurrentWindow(int currentWindow);
    
private:
    class OrientationLabel
    {
    public:
        std::string text;
        cgl::Vec2 position;
        OrientationLabel(std::string text, cgl::Vec2 position) : text(text), position(position) {}
    };
    
    DCMImageSeries* volume;
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
    int currentWindow;
    std::vector<OrientationLabel> labels;
    cgl::Mat4 modelMatrix;
    
    void drawSlice();
    void drawOrientationOverlay();
};

#endif /* defined(__medleap__SliceRenderer2D__) */
