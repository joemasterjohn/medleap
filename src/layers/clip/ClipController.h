#ifndef __medleap_ClipController__
#define __medleap_ClipController__

#include "layers/Controller.h"
#include "leap/VPose.h"
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
	VPose v_pose_;
	LeapCameraControl cam_control_;
	unsigned cur_plane_;
	gl::Vec2 leap_start_;
	gl::Vec2 leap_end_;
	gl::Vec2 leap_current_;
};

#endif // __medleap_ClipController__
