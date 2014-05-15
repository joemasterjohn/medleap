#ifndef __LEAP_POSES_POINT2_H__
#define __LEAP_POSES_POINT2_H__

#include "Pose2H.h"

class PointPose2H  : public Pose2H
{
public:
	PointPose2H();

	const Leap::Finger& leftPointer() const { return left_pointer_; }
	const Leap::Finger& leftPointerPrevious() const { return left_pointer_prev_; }
	const Leap::Finger& leftPointerEngaged() const { return left_pointer_engaged_; }
	const Leap::Finger& rightPointer() const { return right_pointer_; }
	const Leap::Finger& rightPointerPrevious() const { return right_pointer_prev_; }
	const Leap::Finger& rightPointerEngaged() const { return right_pointer_engaged_; }

	Leap::Vector center() const;
	Leap::Vector deltaCenter() const;
	Leap::Vector deltaCenterEngaged() const;

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void engage(const Leap::Frame& frame) override;

private:
	Leap::Finger left_pointer_;
	Leap::Finger left_pointer_prev_;
	Leap::Finger left_pointer_engaged_;
	Leap::Finger right_pointer_;
	Leap::Finger right_pointer_prev_;
	Leap::Finger right_pointer_engaged_;
};

#endif