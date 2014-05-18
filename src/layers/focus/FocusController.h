#ifndef __medleap_FocusController__
#define __medleap_FocusController__

#include "layers/Controller.h"
#include "leap/PointPose.h"
#include "layers/volume/LeapCameraControl.h"

class FocusController : public Controller
{
public:
	FocusController();

	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;
	void gainFocus() override;
	void loseFocus() override;

private:
	LeapCameraControl camera_control_;
	PointPose point_pose_;
	gl::Vec3 cursor_pos_;

	void moveCursor();
};

#endif // __medleap_FocusController__
