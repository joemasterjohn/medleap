#include "FistPose.h"

using namespace Leap;

FistPose::FistPose() : max_engage_spd_(500.0f)
{
	engageDelay(std::chrono::milliseconds(150));
}

bool FistPose::shouldEngage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();
	Hand potential_hand = frame.hands().frontmost();

	if (frame.hands().count() != 1)
		return false;

	if (!potential_hand.isValid())
		return false;

	if (potential_hand.grabStrength() < 0.85f)
		return false;

	if (potential_hand.palmVelocity().magnitude() > max_engage_spd_)
		return false;

	hand_engaged_ = potential_hand;

	return true;
}

bool FistPose::shouldDisengage(const Leap::Controller& controller)
{
	hand_current_ = controller.frame().hand(hand_engaged_.id());

	if (!hand_current_.isValid())
		return true;

	if (hand_current_.grabStrength() < 0.85f)
		return true;


	return false;
}

void FistPose::track(const Leap::Controller& controller)
{
	hand_current_ = controller.frame().hand(hand_engaged_.id());

	PoseTracker::track(controller);
}

Hand FistPose::handEngaged() const
{
	return hand_engaged_;
}

Hand FistPose::handCurrent() const
{
	return hand_current_;
}

Vector FistPose::palmPosEngaged() const
{
	return hand_engaged_.palmPosition();
}

Vector FistPose::palmPosCurrent() const
{
	return hand_current_.palmPosition();
}

Vector FistPose::palmPosDelta() const
{
	return palmPosCurrent() - palmPosEngaged();
}

void FistPose::maxEngageSpeed(float speed)
{
	max_engage_spd_ = speed;
}