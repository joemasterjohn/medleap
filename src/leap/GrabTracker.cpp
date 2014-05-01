#include "GrabTracker.h"

using namespace Leap;

GrabTracker::GrabTracker() : max_engage_spd_(500.0f)
{
	engageDelay(std::chrono::milliseconds(150));
}

bool GrabTracker::shouldEngage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();
	Hand potential_hand = frame.hands().frontmost();

	if (frame.hands().count() != 1)
		return false;

	if (!potential_hand.isValid())
		return false;

	if (controller.frame(20).fingers().count() < 4)
		return false;

	if (potential_hand.fingers().count() > 1)
		return false;

	if (potential_hand.palmPosition().z > 150)
		return false;

	if (potential_hand.palmVelocity().magnitude() > max_engage_spd_)
		return false;

	hand_engaged_ = potential_hand;

	return true;
}

bool GrabTracker::shouldDisengage(const Leap::Controller& controller)
{
	if (!hand_current_.isValid())
		return true;

	if (hand_current_.fingers().count() > 2)
		return true;

	return false;
}

void GrabTracker::track(const Leap::Controller& controller)
{
	hand_current_ = controller.frame().hand(hand_engaged_.id());

	LeapTracker::track(controller);
}

Hand GrabTracker::handEngaged() const
{
	return hand_engaged_;
}

Hand GrabTracker::handCurrent() const
{
	return hand_current_;
}

Vector GrabTracker::palmPosEngaged() const
{
	return hand_engaged_.palmPosition();
}

Vector GrabTracker::palmPosCurrent() const
{
	return hand_current_.palmPosition();
}

Vector GrabTracker::palmPosDelta() const
{
	return palmPosCurrent() - palmPosEngaged();
}

void GrabTracker::maxEngageSpeed(float speed)
{
	max_engage_spd_ = speed;
}