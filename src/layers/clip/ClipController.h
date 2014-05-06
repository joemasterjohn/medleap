#ifndef __medleap_ClipController__
#define __medleap_ClipController__

#include "layers/Controller.h"
#include "leap/GrabTracker.h"
#include "leap/CutTracker.h"
#include "layers/volume/LeapCameraControl.h"

class ClipController : public Controller
{
public:
	ClipController();
	void gainFocus() override;
	void loseFocus() override;
	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;
	std::unique_ptr<Menu> contextMenu() override;
	void draw() override;

private:
	CutTracker cut_tracker_;
	GrabTracker grab_tracker_;
	LeapCameraControl cam_control_;
	unsigned cur_plane_;
	gl::Vec2 leap_start_;
	gl::Vec2 leap_end_;
	gl::Vec2 leap_current_;

	void updateVector(const Leap::Controller& controller);
};

#endif // __medleap_ClipController__
