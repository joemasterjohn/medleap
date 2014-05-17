#include "CarryPose.h"

using namespace Leap;

CarryPose::CarryPose()
{
	minValidFrames(5);
	maxHandEngageSpeed(175.0f);
}

bool CarryPose::shouldEngage(const Frame& frame)
{
	if (!Pose1H::shouldEngage(frame)) {
		return false;
	}

	if (hand().fingers().extended().count() != 5) {
		return false;
	}

	if (hand().palmNormal().dot(Vector::yAxis()) < 0.75f) {
		return false;
	}

	return true;
}

bool CarryPose::shouldDisengage(const Frame& frame)
{
	if (Pose1H::shouldDisengage(frame)) {
		return true;
	}

	if (hand().fingers().extended().count() != 5) {
		return true;
	}

	if (hand().palmNormal().dot(Vector::yAxis()) < 0.75f) {
		return true;
	}

	return false;
}