#ifndef __medleap_ClipController__
#define __medleap_ClipController__

#include "layers/Controller.h"
#include "leap/VPose.h"
#include "layers/volume/LeapCameraControl.h"
#include "gl/geom/Plane.h"

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
	unsigned selected_plane_;
	gl::Vec2 leap_start_;
	gl::Vec2 leap_end_;
	gl::Vec2 leap_current_;

	void clip1H(const Leap::Frame& frame);
	void clip2H(const Leap::Frame& frame);
	void addPlane(const gl::Plane& plane);
	void deleteSelectedPlane();
	void clearPlanes();
	gl::Plane& selectedPlane();
};

#endif // __medleap_ClipController__
