#include "PointPose.h"

using namespace Leap;

PointPose::PointPose() :
	engage_spd_thresh_(50),
	disengage_spd_thresh_(250)
{
}

bool PointPose::shouldEngage(const Leap::Controller& controller)
{
	HandList hands = controller.frame().hands();

	if (hands.count() != 1)
		return false;

	const Hand& hand = hands[0];
	FingerList extended = hand.fingers().extended();
	if (extended.count() != 1 || extended[0].type() != Finger::TYPE_INDEX)
		return false;

	engaged_ = current_ = extended[0];

	if (engaged_.tipVelocity().magnitude() > engage_spd_thresh_)
		return false;

	return true;
}

bool PointPose::shouldDisengage(const Leap::Controller& controller)
{
	HandList hands = controller.frame().hands();
	if (hands.count() != 1)
		return true;

	const Hand& hand = hands[0];
	FingerList extended = hand.fingers().extended();
	if (extended.count() != 1 || extended[0].type() != Finger::TYPE_INDEX)
		return true;

	current_ = extended[0];
	if (current_.tipVelocity().z > disengage_spd_thresh_)
		return true;

	return false;
}

void PointPose::track(const Leap::Controller& controller)
{
	current_ = controller.frame().finger(engaged_.id());
	PoseTracker::track(controller);
}

Vector PointPose::posDelta() const
{
	return index().tipPosition() - indexEngaged().tipPosition();
}