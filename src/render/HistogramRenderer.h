#ifndef __medleap__HistogramRenderer__
#define __medleap__HistogramRenderer__

#include "render/Renderer.h"
#include "gl/Texture.h"
#include "gl/Program.h"
#include "math/Matrix4.h"
#include "volume/VolumeData.h"
#include "volume/Histogram.h"

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
    
private:
    cgl::Texture* histo1D;
    Program* shader;
    GLuint vao;
    GLuint vbo;
    cgl::Mat4 histoModelMatrix;
    // shader program
    // vbo
    // vao
    
};

#endif /* defined(__medleap__HistogramRenderer__) */
