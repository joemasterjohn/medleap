#include "PushPose.h"

using namespace Leap;

PushPose::PushPose() : closed_(false)
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
	bool was_closed = closed_;
	closed_ = hand().grabStrength() > 0.95f;

	if (closed_ && !was_closed) {
		hand_closed_ = hand();
	}
}

Vector PushPose::handPositionClosed(bool stabilized) const
{
	return stabilized ? hand_closed_.stabilizedPalmPosition() : hand_closed_.palmPosition();
}