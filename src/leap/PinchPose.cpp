#include "PinchPose.h"

using namespace Leap;

PinchPose::PinchPose() : 
	pinching_(false), 
	pinch_strength_(0.90f), 
	release_strength_(0.85f),
	open_fn_(nullptr),
	close_fn_(nullptr)
{
	maxHandEngageSpeed(175.0f);
}

bool PinchPose::shouldEngage(const Frame& frame)
{
	if (!Pose1H::shouldEngage(frame)) {
		return false;
	}

	if (hand().fingers().extended().count() < 3) {
		return false;
	}

	if (hand().grabStrength() > 0.25f) {
		return false;
	}

	pinching_ = false;
	return true;
}

bool PinchPose::shouldDisengage(const Frame& frame)
{
	if (Pose1H::shouldDisengage(frame)) {
		return true;
	}

	if (hand().fingers().extended().count() < 3) {
		return true;
	}

	if (hand().grabStrength() > 0.25f) {
		return true;
	}

	return false;
}

void PinchPose::track(const Frame& frame)
{
	if (pinching_) {
		if (hand().confidence() > 0.75f && hand().pinchStrength() <= release_strength_) {
			pinching_ = false;
			if (open_fn_) {
				open_fn_(frame);
			}
		}
	} else {
		if (hand().confidence() > 0.75f && hand().pinchStrength() >= pinch_strength_) {
			hand_pinched_ = hand();
			pinching_ = true;
			if (close_fn_) {
				close_fn_(frame);
			}
		}
	}
}