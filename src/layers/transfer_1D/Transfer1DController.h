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
#include "Transfer1D.h"
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
	float cursor_;
	bool dirty_textures_;
    VolumeController* volumeRenderer;
    SliceController* sliceRenderer;
    std::vector<Transfer1D> transfers_;
	int active_transfer_;
	Transfer1D::Marker* selected_;
	LeapCameraControl camera_control_;
	PoseTracker poses_;
	bool leap_drag_performed_;
	bool scaling_markers_;
	Interval saved_interval_;
	std::vector<float> saved_centers_;

	// rendering
	TextRenderer text;
	GLuint vbo; // TODO: replace with object
	GLsizei stride;
	gl::Buffer bgBuffer;
	gl::Program bgShader;
	gl::Program histoProg;
	gl::Program histoOutlineProg;
	gl::Buffer  histoVBO;
	gl::Draw histoOutline;
	GLsizei histoVBOCount;
	gl::Texture clutTexture;
	gl::Texture contextTexture;
	gl::Draw colorStops;

	Transfer1D& transfer();
	void drawMarkerBar();
	void drawBackground();
	void drawHistogram();

	void nextCLUT();
	void prevCLUT();
	void markDirty();
	void addMarker();
	void updateTextures();
	void chooseSelected();
	void moveSelected();
	void scaleSelected(float width);
	void deleteSelected();
	void changeColorSelected();
	void toggleContextSelected();
	void saveInterval();
	void scaleAllMarkers();

	void createFunction();
	void deleteFunction();
	void toggleGradient();
};

#endif /* defined(__medleap__Transfer1DController__) */
