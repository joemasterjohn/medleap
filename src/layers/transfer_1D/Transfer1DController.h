#ifndef __medleap__Transfer1DController__
#define __medleap__Transfer1DController__

#include "layers/Controller.h"
#include "Transfer1DRenderer.h"
#include "data/VolumeData.h"
#include "layers/volume/VolumeRenderer.h"
#include "layers/slice/SliceRenderer.h"
#include "Histogram.h"
#include "leap/HandTriggerTracker.h"
#include "leap/TwoFingerTracker.h"

class Transfer1DController : public Controller
{
public:
    Transfer1DController();
    ~Transfer1DController();
    Transfer1DRenderer* getRenderer();
    void setVolume(VolumeData* volume);
    
    bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
    bool mouseMotion(GLFWwindow* window, double x, double y) override;
    bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;
	std::set<Leap::Gesture::Type> requiredGestures() override;
    Histogram* histogram;

    void setVolumeRenderer(VolumeRenderer* volumeRenderer);
    void setSliceRenderer(SliceRenderer* sliceRenderer);
    
private:
    bool lMouseDrag, rMouseDrag;
    Transfer1DRenderer renderer;
    VolumeData* volume;
    GLubyte* transfer1DPixels;

    
    VolumeRenderer* volumeRenderer;
    SliceRenderer* sliceRenderer;
    std::vector<CLUT> cluts;
    int activeCLUT;
	CLUT::Marker* selected_;

	// leap
	HandTriggerTracker trigger_tracker_;
	TwoFingerTracker finger_tracker_;
	Interval saved_interval_;

    void updateTransferTex1D();

	void moveAndScale(const Leap::Controller& controller);

	void nextCLUT();
	void prevCLUT();
};

#endif /* defined(__medleap__Transfer1DController__) */
