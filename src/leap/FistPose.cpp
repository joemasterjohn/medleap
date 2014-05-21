#include "FistPose.h"

using namespace Leap;

FistPose::FistPose() : state_(State::open)
{
	maxHandEngageSpeed(175.0f);
}

bool FistPose::shouldEngage(const Frame& frame)
{
	if (!Pose1H::shouldEngage(frame)) {
		return false;
	}

	if (hand().fingers().extended().count() != 5) {
		return false;
	}

	return true;
}

bool FistPose::shouldDisengage(const Frame& frame)
{
	if (Pose1H::shouldDisengage(frame)) {
		return true;
	}

	return false;
}

void FistPose::track(const Frame& frame)
{
	if (hand().grabStrength() == 1.0f && hand().fingers().extended().count() == 0) {
		state_ = State::closed;
	} else if (hand().grabStrength() < 1.0f && hand().fingers().extended().count() == 5) {
		state_ = State::open;
	} else {
		state_ = State::partial;
	}
}