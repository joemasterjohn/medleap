#ifndef __medleap__VolumeRenderer__
#define __medleap__VolumeRenderer__

#include "gl/glew.h"
#include "math/Matrix4.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "gl/Buffer.h"
#include "gl/Framebuffer.h"
#include "data/VolumeData.h"
#include "util/Camera.h"
#include "layers/Renderer.h"

class VolumeRenderer : public Renderer
{
public:
    enum RenderMode { MIP, VR, ISOSURFACE, NUM_OF_MODES };
    
    
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
    
    void setOpacityScale(float scale);
    float getOpacityScale();
    
private:
    bool shading;
    RenderMode renderMode;
    int numSamples;
    float movingSampleScale;
    bool dirty;
    bool drawnHighRes;
    bool moving;
    gl::Texture* volumeTexture;
    gl::Texture* gradientTexture;
    VolumeData* volume;
    Camera camera;
    Mat4 model;
    float opacityScale;
    
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
    
    // render to texture 
    gl::Framebuffer* lowResFBO;
    gl::Texture* lowResTexture;
    gl::Framebuffer* fullResFBO;
    gl::Texture* fullResTexture;
    gl::Program* sceneProgram;
    gl::Buffer* sceneBuffer;
    
    void updateSlices(int numSlices);
    
    void draw(int numSlices);
};

#endif /* defined(__medleap__VolumeRenderer__) */
