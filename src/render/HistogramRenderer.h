#ifndef __medleap__HistogramRenderer__
#define __medleap__HistogramRenderer__

#include "render/Renderer.h"
#include "gl/Texture.h"
#include "gl/Program.h"
#include "math/Matrix4.h"
#include "volume/VolumeData.h"
#include "volume/Histogram.h"
#include "util/TextRenderer.h"

class HistogramRenderer : public Renderer
{
public:
    HistogramRenderer();
    ~HistogramRenderer();
    void init();
    void draw();
    void resize(int width, int height);
    void setVolume(VolumeData* volume);
    void setHistogram(Histogram* histogram);
    
    void setCursor(int x, int y);
    
    gl::Texture* getTransferFn();
    
    void setDrawCursor(bool draw);
    
private:
    bool drawCursor;
    VolumeData* volume;
    TextRenderer text;
    gl::Texture* histo1D;
    gl::Texture* transferFn;
    gl::Program* shader;
    gl::Program* colorShader;
    GLuint vbo;
    GLsizei stride;
    Mat4 histoModelMatrix;
    Histogram* histogram;
    int cursorX, cursorY;
    float cursorShaderOffset;
    float cursorValue;
    
    void drawWindowMarkers();
    void drawCursorValue();
};

#endif /* defined(__medleap__HistogramRenderer__) */
