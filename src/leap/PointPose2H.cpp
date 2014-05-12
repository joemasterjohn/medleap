#include "PointPose2H.h"

using namespace Leap;

PointPose2H::PointPose2H() :
		engage_spd_thresh_(50),
		disengage_spd_thresh_(250)
{
	engageDelay(std::chrono::milliseconds(250));
}

bool PointPose2H::shouldEngage(const Leap::Controller& controller)
{
	HandList hands = controller.frame().hands();
	if (hands.count() != 2)
		return false;

	if (hands[0].isLeft()) {
		left_engaged_ = left_current_ = hands[0];
		right_engaged_ = right_current_ = hands[1];
	} else {
		left_engaged_ = left_current_ = hands[1];
		right_engaged_ = right_current_ = hands[0];
	}

	FingerList l_extended = left_current_.fingers().extended();
	if (l_extended.count() != 1 || l_extended[0].type() != Finger::TYPE_INDEX)
		return false;

	FingerList r_extended = right_current_.fingers().extended();
	if (r_extended.count() != 1 || r_extended[0].type() != Finger::TYPE_INDEX)
		return false;

	if (leftIndex().tipVelocity().magnitude() > engage_spd_thresh_)
		return false;

	if (rightIndex().tipVelocity().magnitude() > engage_spd_thresh_)
		return false;

	return true;
}

bool PointPose2H::shouldDisengage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();
	HandList hands = frame.hands();

	if (hands.count() != 2)
		return true;

	FingerList l_extended = left_current_.fingers().extended();
	if (l_extended.count() != 1 || l_extended[0].type() != Finger::TYPE_INDEX)
		return true;

	FingerList r_extended = right_current_.fingers().extended();
	if (r_extended.count() != 1 || r_extended[0].type() != Finger::TYPE_INDEX)
		return true;

	// may be better to use velocity perp. to hand axis, not to camera
	if (leftIndex().tipVelocity().z > disengage_spd_thresh_)
		return true;

	if (rightIndex().tipVelocity().z > disengage_spd_thresh_)
		return true;

	return false;
}

void PointPose2H::track(const Leap::Controller& controller)
{
	Frame frame = controller.frame();
	left_current_ = frame.hand(left_engaged_.id());
	right_current_ = frame.hand(right_engaged_.id());
	PoseTracker::track(controller);
}

float PointPose2H::fingerGap() const
{
	Vector l = leftIndex().tipPosition();
	Vector r = rightIndex().tipPosition();
	return (l - r).magnitude();
}

float PointPose2H::fingerGapEngaged() const
{
	Vector l = leftIndexEngaged().tipPosition();
	Vector r = rightIndexEngaged().tipPosition();
	return (l - r).magnitude();
}

float PointPose2H::fingerGapDelta() const
{
	return fingerGap() - fingerGapEngaged();
}

Vector PointPose2H::center() const
{
	Vector l = leftIndex().tipPosition();
	Vector r = rightIndex().tipPosition();
	return (l + r) / 2.0f;
}

Vector PointPose2H::centerEngaged() const
{
	Vector l = leftIndexEngaged().tipPosition();
	Vector r = rightIndexEngaged().tipPosition();
	return (l + r) / 2.0f;
}

Vector PointPose2H::centerDelta() const
{
	return center() - centerEngaged();
}