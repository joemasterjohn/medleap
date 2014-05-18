#ifndef __LEAP_POSES_TRACKER_H__
#define __LEAP_POSES_TRACKER_H__

#include "CarryPose.h"
#include "FistPose.h"
#include "LPose.h"
#include "PalmsFacePose.h"
#include "PinchPose.h"
#include "PointPose.h"
#include "PointPose2H.h"
#include "VPose.h"
#include "PushPose.h"

class PoseTracker
{
public:
	PoseTracker();

	void enableAll(bool enabled);
	void update(const Leap::Frame& frame);

	CarryPose& carry() { return carry_; }
	FistPose& fist() { return fist_; }
	LPose& l() { return l_; }
	PalmsFacePose& palmsFace() { return palms_face_; }
	PinchPose& pinch() { return pinch_; }
	PointPose& point() { return point_; }
	PointPose2H& point2() { return point2_; }
	VPose& v() { return v_; }
	PushPose& push() { return push_; }

private:
	CarryPose carry_;
	FistPose fist_;
	LPose l_;
	PalmsFacePose palms_face_;
	PinchPose pinch_;
	PointPose point_;
	PointPose2H point2_;
	VPose v_;
	PushPose push_;
};

#endif