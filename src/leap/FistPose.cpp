#include "FistPose.h"

using namespace Leap;

FistPose::FistPose() : state_(State::open)
{
	minValidFrames(5);
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
	if (hand().grabStrength() < 0.95f) {
		if (hand().fingers().extended().count() == 5) {
			state_ = State::open;
		} else {
			state_ = State::partial;
		}
	} else {
		state_ = State::closed;
	}
}