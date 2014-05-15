#ifndef __LEAP_POSES_POINT_H__
#define __LEAP_POSES_POINT_H__

#include "Pose1H.h"

class PointPose  : public Pose1H
{
public:
	PointPose(Pose1H::TrackedHand hand = Pose1H::TrackedHand::first);

	const Leap::Finger& pointer() const { return pointer_; }
	const Leap::Finger& pointerEngaged() const { return pointer_engaged_; }

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void engage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
	Leap::Finger pointer_;
	Leap::Finger pointer_engaged_;
};

#endif