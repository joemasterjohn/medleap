#ifndef __MEDLEAP_LEAP_CAMERA_CONTROL_H__
#define __MEDLEAP_LEAP_CAMERA_CONTROL_H__

#include "Leap.h"
#include "leap/PoseTracker.h"
#include "gl/math/Math.h"
#include "util/Camera.h"

class LeapCameraControl
{
public:
	LeapCameraControl();
	void update(const Leap::Controller& controller, const Leap::Frame& frame);
	void mouseButton(int button, int action, int mods, double x, double y);
	void mouseMotion(double x, double y);
	void mouseScroll(double x, double y);
	bool tracking() const { return tracking_; }

private:
	PoseTracker poses_;
	bool tracking_;
	bool mouse_drag_l_;
	bool mouse_drag_r_;
	double mouse_last_x_;
	double mouse_last_y_;
	double mouse_drag_start_x_;
	double mouse_drag_start_y_;

	Camera& camera();

	void rotate(float delta_yaw, float delta_pitch);
	void move(const gl::Vec3& delta);
	void zoom(float delta_radius);
};

#endif
