#ifndef __MEDLEAP_LEAP_CAMERA_CONTROL_H__
#define __MEDLEAP_LEAP_CAMERA_CONTROL_H__

#include "Leap.h"
#include "leap/FistPose.h"
#include "leap/PointPose2H.h"
#include "gl/math/Math.h"
#include "util/Camera.h"

class LeapCameraControl
{
public:
	LeapCameraControl();
	void update(const Leap::Controller& controller);

	bool tracking() const { return tracking_; }

private:
	FistPose fist_pose_;
	PointPose2H point_pose_;
	gl::Mat4 old_view_;
	Camera old_camera_;
	bool tracking_;

	void grab(const Leap::Frame& controller);
	void release(const Leap::Frame& controller);
};

#endif
