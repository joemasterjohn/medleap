#ifndef __medleap__Transfer1DController__
#define __medleap__Transfer1DController__

#include "layers/Controller.h"
#include "data/VolumeData.h"
#include "layers/volume/VolumeController.h"
#include "layers/slice/SliceController.h"
#include "gl/Texture.h"
#include "gl/Program.h"
#include "gl/Buffer.h"
#include "gl/math/Math.h"
#include "Histogram.h"
#include "CLUT.h"
#include "leap/HandTriggerTracker.h"
#include "leap/TwoFingerTracker.h"
#include "util/TextRenderer.h"
#include "gl/util/Draw.h"

class Transfer1DController : public Controller
{
public:
    Transfer1DController();
    ~Transfer1DController();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
    bool mouseMotion(GLFWwindow* window, double x, double y) override;
    bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;
	std::set<Leap::Gesture::Type> requiredGestures() override;
	std::unique_ptr<Menu> contextMenu() override;

	void draw() override;

    void setVolumeRenderer(VolumeController* volumeRenderer);
	void setSliceRenderer(SliceController* sliceRenderer);
    
private:
    bool lMouseDrag, rMouseDrag;
    VolumeData* volume;
    GLubyte* transfer1DPixels;

    
    VolumeController* volumeRenderer;
    SliceController* sliceRenderer;
    std::vector<CLUT> cluts;
    int activeCLUT;
	CLUT::Marker* selected_;

	// leap
	HandTriggerTracker trigger_tracker_;
	TwoFingerTracker finger_tracker_;
	Interval saved_interval_;

	// rendering
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
	gl::Program bgShader;
	gl::Buffer bgBuffer;
	gl::Program histoProg;
	gl::Program histoOutlineProg;
	gl::Buffer  histoVBO;
	gl::Draw histoOutline;
	GLsizei histoVBOCount;
	gl::Texture clutTexture;
	gl::Draw colorStops;

	void moveAndScale(const Leap::Controller& controller);

	void nextCLUT();
	void prevCLUT();

	void drawMarkerBar();
	void drawBackground();
	void drawHistogram();

};

#endif /* defined(__medleap__Transfer1DController__) */
