#ifndef __medleap__VolumeController__
#define __medleap__VolumeController__

#include "layers/Controller.h"
#include "gl/math/Math.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "gl/Buffer.h"
#include "gl/Framebuffer.h"
#include "data/VolumeData.h"
#include "util/Camera.h"
#include "gl/Renderbuffer.h"
#include "gl/util/RenderTarget.h"
#include "gl/util/FullScreenQuad.h"
#include "gl/geom/Plane.h"
#include "LeapCameraControl.h"
#include "gl/geom/Sphere.h"

/** Main controller for 3D mode */
class VolumeController : public Controller
{
public:
	enum RenderMode { MIP, VR, ISOSURFACE, NUM_OF_MODES };

    VolumeController();

	Camera& getCamera();

    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
    bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
    bool mouseMotion(GLFWwindow* window, double x, double y) override;
    bool scroll(GLFWwindow* window, double dx, double dy) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;
	std::unique_ptr<Menu> contextMenu();

	void gainFocus() override;
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

	void draw() override;

	// TODO: cleanup
	float cursorRadius;
	bool cursorActive;
	gl::Buffer cursor3DVBO;
	gl::Program cursor3DShader;
	bool useJitter;
	gl::Vec3 maskColor;
	gl::Vec3 maskCenter;
	bool draw_bounds;
	bool draw_planes;
	bool draw_cursor3D;
	bool use_context;
    bool draw_lines;

	gl::Texture maskTexture;
	gl::Geometry maskGeometry;

	gl::Texture tex_context_;
    float isovalue;
	std::vector<gl::Plane>& clipPlanes() { return clip_planes_; }

private:
    VolumeData* volume;
   

	std::vector<gl::Plane> clip_planes_;

	bool shading;
	RenderMode renderMode;
	bool dirty;
	bool drawnHighRes;
	gl::Texture volumeTexture;
	gl::Texture gradientTexture;
	gl::Texture jitterTexture;
	Camera camera;
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

	LeapCameraControl leap_cam_control_;

	void resize() override;
	void updateSlices(double samplingScale, bool limitSamples);
	void draw(double samplingScale, bool limitSamples, int width, int height);
};

#endif /* defined(__medleap__VolumeController__) */
