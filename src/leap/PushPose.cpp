#include "PushPose.h"

using namespace Leap;

PushPose::PushPose()
{
	maxHandEngageSpeed(175.0f);
}

bool PushPose::shouldEngage(const Frame& frame)
{
	if (!Pose1H::shouldEngage(frame)) {
		return false;
	}

	if (hand().fingers().extended().count() != 5) {
		return false;
	}

	if (hand().palmNormal().dot(-Vector::zAxis()) < 0.7f) {
		return false;
	}

	return true;
}

bool PushPose::shouldDisengage(const Frame& frame)
{
	if (Pose1H::shouldDisengage(frame)) {
		return true;
	}

	if (hand().palmNormal().dot(-Vector::zAxis()) < 0.7f) {
		return true;
	}

	return false;
}

void PushPose::track(const Frame& frame)
{
	closed_ = hand().grabStrength() > 0.95f;
}