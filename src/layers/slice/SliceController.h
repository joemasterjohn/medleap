#ifndef __medleap__SliceController__
#define __medleap__SliceController__

#include "gl/glew.h"
#include "layers/Controller.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "gl/Buffer.h"
#include "data/VolumeData.h"
#include "gl/math/Math.h"
#include "leap/TwoFingerTracker.h"

/** Controls slice rendering layer */
class SliceController : public Controller
{
public:
    SliceController();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
    bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
    bool mouseMotion(GLFWwindow* window, double x, double y) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;
	void setCLUTTexture(gl::Texture& texture);
	void update(std::chrono::milliseconds elapsed) override;
	int slice() const { return currentSlice_; }
	void slice(int index);
	void gainFocus() override;
	void loseFocus() override;
	void draw() override;
    
private:
    VolumeData* volume;
    
    bool mouseLeftDrag;
    double mouseAnchorX;
    double mouseAnchorY;
    int anchorSliceIndex;

	gl::Program sliceShader;
	gl::Texture clutTexture;
	gl::Texture sliceTexture;
	gl::Buffer sliceVBO;
	int currentSlice_;
	gl::Mat4 modelMatrix;
	std::chrono::milliseconds elapsed_;
	TwoFingerTracker finger_tracker_;
	int saved_slice_;
	float leap_scroll_dst_;

	void resize() override;
	void updateTexture();

	void leapScroll(const Leap::Controller& controller);
};

#endif /* defined(__medleap__SliceController__) */
