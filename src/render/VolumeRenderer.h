#ifndef __medleap__VolumeRenderer__
#define __medleap__VolumeRenderer__

#include "gl/glew.h"
#include "math/Matrix4.h"
#include "gl/Viewport.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "gl/Buffer.h"
#include "gl/Framebuffer.h"
#include "volume/VolumeData.h"
#include "util/Camera.h"
#include "render/Renderer.h"

class VolumeRenderer : public Renderer
{
public:
    enum RenderMode { MIP, VR };
    
    
    VolumeRenderer();
    ~VolumeRenderer();
    void init();
    void resize(int width, int height);
    void draw();
    
    void setVolume(VolumeData* volume);
    
    Camera& getCamera();
    
    void markDirty();
    void setMoving(bool moving);
    
    void setMode(RenderMode mode);
    void cycleMode();
    RenderMode getMode();
    
    bool useShading();
    void toggleShading();
    int getNumSamples();
    
    void setCLUTTexture(gl::Texture* texture);
    
private:
    bool shading;
    RenderMode renderMode;
    int numSamples;
    float movingSampleScale;
    bool dirty;
    bool moving;
    gl::Texture* volumeTexture;
    gl::Texture* gradientTexture;
    VolumeData* volume;
    Camera camera;
    gl::Viewport viewport;
    Mat4 model;
    
    // color look-up table texture
    gl::Texture* clutTexture;
    
    // grid
    gl::Program* lineShader;
    GLuint vao;
    GLuint vbo;
    int numGridVerts;
    
    // proxy geometry
    gl::Program* boxShader;
    int numSliceIndices;
    gl::Buffer* proxyVertices;
    gl::Buffer* proxyIndices;
    
    // render to texture (downscaling)
    gl::Framebuffer* sceneFramebuffer;
    gl::Texture* sceneTexture;
    gl::Program* sceneProgram;
    gl::Buffer* sceneBuffer;
    
    void updateSlices();
};

#endif /* defined(__medleap__VolumeRenderer__) */
