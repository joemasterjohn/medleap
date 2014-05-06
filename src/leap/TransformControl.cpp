#include "TransformControl.h"

using namespace Leap;

TransformControl::TransformControl()
{
	engageDelay(std::chrono::milliseconds(150));
}

bool TransformControl::shouldEngage(const Leap::Controller& controller)
{
	return true;
}

bool TransformControl::shouldDisengage(const Leap::Controller& controller)
{
	return false;
}

void TransformControl::track(const Leap::Controller& controller)
{
	LeapTracker::track(controller);
}