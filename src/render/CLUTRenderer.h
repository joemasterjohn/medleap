#ifndef __medleap__CLUTRenderer__
#define __medleap__CLUTRenderer__

#include "Renderer.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "volume/CLUT.h"

class CLUTRenderer : public Renderer
{
public:
    CLUTRenderer();
    ~CLUTRenderer();
    void init();
    void draw();
    void resize(int width, int height);
    void setCLUT(CLUT* clut);
    cgl::Texture* getTexture();
    
private:
    Program* quadShader;
    cgl::Buffer* quadVBO;
    cgl::Texture* clutTexture;
    CLUT* clut;
    int stride;
};

#endif /* defined(__medleap__CLUTRenderer__) */