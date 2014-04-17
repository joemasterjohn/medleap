#ifndef __medleap__VolumeRenderer__
#define __medleap__VolumeRenderer__

#include "gl/glew.h"
#include "gl/math/Matrix4.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "gl/Buffer.h"
#include "gl/Framebuffer.h"
#include "data/VolumeData.h"
#include "util/Camera.h"
#include "layers/Renderer.h"
#include "gl/Renderbuffer.h"
#include "gl/util/RenderTarget.h"
#include "gl/util/FullScreenQuad.h"
#include "gl/geom/Sphere.h"

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
    
    void setMode(RenderMode mode);
    void cycleMode();
    RenderMode getMode();
    
    bool useShading();
    void toggleShading();
    
    void setCLUTTexture(gl::Texture& texture);
    
    void setOpacityScale(float scale);
    float getOpacityScale();

	unsigned getCurrentNumSlices();

	// TODO: cleanup
	float cursorRadius;
	bool cursorActive;
	gl::Buffer cursor3DVBO;
	gl::Program cursor3DShader;
	Vec3 cursor3D;
	gl::Sphere cursorGeom;
	bool useJitter;
    
private:
    bool shading;
    RenderMode renderMode;
    bool dirty;
    bool drawnHighRes;
    gl::Texture volumeTexture;
    gl::Texture gradientTexture;
	gl::Texture jitterTexture;
    VolumeData* volume;
    Camera camera;
    Mat4 model;
    float opacityScale;
	unsigned minSlices;
	unsigned maxSlices;
	unsigned currentNumSlices;
    
    gl::Texture clutTexture;
        
    // proxy geometry
    gl::Program boxShader;
    int numSliceIndices;
    gl::Buffer proxyVertices;
    gl::Buffer proxyIndices;
    
    // render to texture 
	gl::RenderTarget fullResRT;
	gl::RenderTarget lowResRT;
	gl::FullScreenQuad fullScreenQuad;
    
    void updateSlices(double samplingScale, bool limitSamples);
    
    void draw(double samplingScale, bool limitSamples, int width, int height);
};

#endif /* defined(__medleap__VolumeRenderer__) */
