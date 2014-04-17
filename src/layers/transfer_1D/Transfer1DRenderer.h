#ifndef __medleap__Transfer1DRenderer__
#define __medleap__Transfer1DRenderer__

#include "layers/Renderer.h"
#include "gl/Texture.h"
#include "gl/Program.h"
#include "gl/Buffer.h"
#include "gl/math/Matrix4.h"
#include "data/VolumeData.h"
#include "Histogram.h"
#include "CLUT.h"
#include "util/TextRenderer.h"

class Transfer1DRenderer : public Renderer
{
public:
    Transfer1DRenderer();
    ~Transfer1DRenderer();
    void init();
    void draw();
    void resize(int width, int height);
    void setVolume(VolumeData* volume);
    void setHistogram(Histogram* histogram);
    void setCLUT(CLUT* clut);
    
    gl::Texture& getCLUTTexture();
    
    void setCursor(int x, int y);
    
    gl::Texture& getTransferFn();
    
    void setDrawCursor(bool draw);
    
private:
    bool drawCursor;
    VolumeData* volume;
    TextRenderer text;
    gl::Texture histo1D;
    gl::Texture transferFn;
    gl::Program shader;
    gl::Program colorShader;
    GLuint vbo; // TODO: replace with object
    GLsizei stride;
    Mat4 histoModelMatrix;
    Histogram* histogram;
    int cursorX, cursorY;
    float cursorShaderOffset;
    float cursorValue;
    
	// BG panel
	gl::Program bgShader;
	gl::Buffer bgBuffer;

    // CLUT
    gl::Program clutStripShader;
    gl::Buffer clutStripVBO;
    gl::Buffer clutStripIBO;
    gl::Texture clutTexture;
    GLsizei clutStripStride;
    CLUT* clut;
    
	void drawBackground();
	void drawHistogram();
	void drawColorBar();
    void drawWindowMarkers();
    void drawCursorValue();
};

#endif /* defined(__medleap__Transfer1DRenderer__) */
