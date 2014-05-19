#ifndef __medleap__Transfer1DController__
#define __medleap__Transfer1DController__

#include "layers/Controller.h"
#include "data/VolumeData.h"
#include "layers/volume/VolumeController.h"
#include "layers/slice/SliceController.h"
#include "layers/volume/LeapCameraControl.h"
#include "gl/Texture.h"
#include "gl/Program.h"
#include "gl/Buffer.h"
#include "gl/math/Math.h"
#include "CLUT.h"
#include "leap/PoseTracker.h"
#include "util/TextRenderer.h"
#include "gl/util/Draw.h"

class Transfer1DController : public Controller
{
public:
    Transfer1DController();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
    bool mouseMotion(GLFWwindow* window, double x, double y) override;
    bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;
	std::set<Leap::Gesture::Type> requiredGestures() override;
	std::unique_ptr<Menu> contextMenu() override;


	void gainFocus() override;
	void loseFocus() override;

	void draw() override;

    void setVolumeRenderer(VolumeController* volumeRenderer);
	void setSliceRenderer(SliceController* sliceRenderer);
    
private:
    bool lMouseDrag, rMouseDrag;
    VolumeData* volume;
	float cursor_center_;
    
	bool dirty_textures_;

    VolumeController* volumeRenderer;
    SliceController* sliceRenderer;
    std::vector<CLUT> cluts;
    int activeCLUT;
	CLUT::Marker* selected_;

	// leap
	LeapCameraControl camera_control_;
	PoseTracker poses_;
	Interval saved_interval_;
	float saved_hand_sep_;
	gl::Vec2 leap_cursor_;
	bool leap_drag_performed_;

	// rendering
	TextRenderer text;
	gl::Texture histo1D;
	gl::Texture transferFn;
	gl::Program shader;
	gl::Program colorShader;
	GLuint vbo; // TODO: replace with object
	GLsizei stride;
	gl::Mat4 histoModelMatrix;
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
	gl::Texture contextTexture;
	gl::Draw colorStops;

	CLUT& currentCLUT();

	void drawMarkerBar();
	void drawBackground();
	void drawHistogram();

	void nextCLUT();
	void prevCLUT();
	void markDirty();
	void updateTextures();
	void updateSelected(float cursor);
	void toggleSelectedContext();
	void moveSelected();
	void scaleSelected(float width);
	void addMarker();
	void deleteMarker();
};

#endif /* defined(__medleap__Transfer1DController__) */
