#include "OneFingerTracker.h"

using namespace Leap;

OneFingerTracker::OneFingerTracker() :
		engage_spd_thresh_(50),
		disengage_spd_thresh_(250)
{
	//engageDelay(std::chrono::milliseconds(200));
}

bool OneFingerTracker::shouldEngage(const Leap::Controller& controller)
{
	HandList hands = controller.frame().hands();

	if (hands.count() != 1)
		return false;

	if (controller.frame().fingers().count() > 3)
		return false;

	finger_ = hands[0].fingers().frontmost();

	if (finger_.tipVelocity().magnitude() > engage_spd_thresh_)
		return false;

	return true;
}

bool OneFingerTracker::shouldDisengage(const Leap::Controller& controller)
{
	HandList hands = controller.frame().hands();

	if (!finger(controller.frame()).isValid())
		return true;

	if (hands.count() != 1)
		return true;

	if (finger(controller.frame()).tipVelocity().z > disengage_spd_thresh_)
		return true;

	return false;
}

Finger OneFingerTracker::finger() const
{
	return finger_;
}

Finger OneFingerTracker::finger(const Leap::Frame& frame) const
{
	return frame.finger(finger_.id());
}

Vector OneFingerTracker::posDelta(const Leap::Frame& frame) const
{
	return finger(frame).tipPosition() - finger().tipPosition();
}