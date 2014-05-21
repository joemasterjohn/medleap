#include "FistPose.h"

using namespace Leap;
using namespace std::chrono;

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
	int num_extended = hand().fingers().extended().count();
	float grab = hand().grabStrength();

	State prev = state_;
	state_ = State::partial;
	if (num_extended == 5) {
		if (grab == 0.0f) {
			state_ = State::open;
		}
	} else if (num_extended == 3) {
		if (grab == 0.0f && hand().fingers()[0].isExtended() && hand().fingers()[1].isExtended() && hand().fingers()[2].isExtended()) {
			state_ = State::three_out;
		}
	} else if (num_extended == 1) {
		if (grab == 1.0f && hand().fingers()[Finger::TYPE_THUMB].isExtended()) {
			state_ = State::thumb_out;
		}
	} else if (num_extended == 0) {
		if (grab == 1.0f) {
			state_ = State::closed;
		}
	}

	if (prev != state_) {
		last_change_ = high_resolution_clock::now();
		time_since_change_ = milliseconds::zero();
	} else {
		time_since_change_ = duration_cast<milliseconds>(high_resolution_clock::now() - last_change_);
	}
}