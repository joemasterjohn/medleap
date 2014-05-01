#ifndef __MEDLEAP_LEAP_CAMERA_CONTROL_H__
#define __MEDLEAP_LEAP_CAMERA_CONTROL_H__

#include "Leap.h"
#include "leap/GrabTracker.h"
#include "gl/math/Math.h"

class LeapCameraControl
{
public:
	LeapCameraControl();
	void update(const Leap::Controller& controller);

private:
	GrabTracker grab_tracker_;
	gl::Mat4 old_view_;

	void grab(const Leap::Controller& controller);
	void release(const Leap::Controller& controller);
};

#endif
