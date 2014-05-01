#include "GrabTracker.h"

using namespace Leap;

GrabTracker::GrabTracker() :
		engage_spd_thresh_(50),
		disengage_spd_thresh_(250)
{
	engageDelay(std::chrono::milliseconds(500));
}

bool GrabTracker::shouldEngage(const Leap::Controller& controller)
{
	HandList hands = controller.frame().hands();

	if (hands.count() != 1)
		return false;

	hand_ = hands.frontmost();

	// previous frame should have 4 or 5 fingers showing
	if (controller.frame(20).fingers().count() < 4)
		return false;

	// current frame should have 1 finger only
	if (controller.frame().fingers().count() > 1)
		return false;

	// don't engage if hand retreating
	if (hand_.palmVelocity().z > 100)
		return false;

	return true;
}

bool GrabTracker::shouldDisengage(const Leap::Controller& controller)
{
	if (controller.frame().fingers().count() > 1)
		return true;


	return false;
}

Hand GrabTracker::hand() const
{
	return hand_;
}

Vector GrabTracker::palmPos() const
{
	return hand_.stabilizedPalmPosition();
}

Vector GrabTracker::palmPos(const Leap::Frame& frame) const
{
	return frame.hand(hand_.id()).palmPosition();
}

Vector GrabTracker::palmPosDelta(const Leap::Frame& frame) const
{
	return palmPos(frame) - palmPos();
}

void GrabTracker::engageSpeedThreshold(float speed)
{
	engage_spd_thresh_ = speed;
}

void GrabTracker::disengageSpeedThreshold(float speed)
{
	disengage_spd_thresh_ = speed;
}