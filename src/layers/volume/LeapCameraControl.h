#ifndef __MEDLEAP_LEAP_CAMERA_CONTROL_H__
#define __MEDLEAP_LEAP_CAMERA_CONTROL_H__

#include "Leap.h"
#include "leap/GrabTracker.h"
#include "leap/TwoFingerTracker.h"
#include "gl/math/Math.h"
#include "util/Camera.h"

class LeapCameraControl
{
public:
	LeapCameraControl();
	void update(const Leap::Controller& controller);

	bool tracking() const { return tracking_; }

private:
	GrabTracker grab_tracker_;
	TwoFingerTracker finger_tracker_;
	gl::Mat4 old_view_;
	Camera old_camera_;
	bool tracking_;

	void grab(const Leap::Controller& controller);
	void release(const Leap::Controller& controller);
};

#endif
