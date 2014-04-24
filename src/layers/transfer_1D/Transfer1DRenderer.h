#ifndef __medleap__Transfer1DRenderer__
#define __medleap__Transfer1DRenderer__

#include "layers/Renderer.h"
#include "gl/Texture.h"
#include "gl/Program.h"
#include "gl/Buffer.h"
#include "gl/math/Math.h"
#include "data/VolumeData.h"
#include "Histogram.h"
#include "CLUT.h"
#include "util/TextRenderer.h"
#include "gl/util/Draw.h"

class Transfer1DRenderer : public Renderer
{
public:
    Transfer1DRenderer();
    ~Transfer1DRenderer();
    void draw();
    void resize(int width, int height);
    void setVolume(VolumeData* volume);
    void setHistogram(Histogram* histogram);
    void setCLUT(CLUT* clut);
    
    gl::Texture& getCLUTTexture();
    
    void setCursor(int x, int y);
    
    gl::Texture& getTransferFn();
        
private:
    VolumeData* volume;
    TextRenderer text;
    gl::Texture histo1D;
    gl::Texture transferFn;
    gl::Program shader;
    gl::Program colorShader;
    GLuint vbo; // TODO: replace with object
    GLsizei stride;
	gl::Mat4 histoModelMatrix;
    Histogram* histogram;
    int cursorX, cursorY;
    float cursorShaderOffset;
    float cursorValue;
    
	// BG panel
	gl::Program bgShader;
	gl::Buffer bgBuffer;

	// histogram
	gl::Program histoProg;
	gl::Program histoOutlineProg;
	gl::Buffer  histoVBO;
	gl::Draw histoOutline;
	GLsizei histoVBOCount;

    gl::Texture clutTexture;
    CLUT* clut;

	// color stops
	gl::Draw colorStops;
    
	void drawMarkerBar();
	void drawBackground();
	void drawHistogram();
};

#endif /* defined(__medleap__Transfer1DRenderer__) */
