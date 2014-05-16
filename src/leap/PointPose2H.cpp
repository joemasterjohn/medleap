#include "PointPose2H.h"

using namespace Leap;

PointPose2H::PointPose2H()
{
	minValidFrames(0);
	maxHandEngageSpeed(100.0f);
}

bool PointPose2H::shouldEngage(const Frame& frame)
{
	if (!Pose2H::shouldEngage(frame)) {
		return false;
	}

	// common issues:
	// - thumb is sometimes incorrectly perceived as extended
	// - middle finger is sometimes incorrectly perceived as extended
	// workarounds:
	// - permit up to 2 extended fingers per hand

	if (left().fingers().extended().count() > 2) {
		return false;
	}

	if (right().fingers().extended().count() > 2) {
		return false;
	}

	left_pointer_ = left().fingers().frontmost();
	if (!left_pointer_.isExtended() || !left_pointer_.isValid()) {
		return false;
	}

	right_pointer_ = right().fingers().frontmost();
	if (!right_pointer_.isExtended() || !right_pointer_.isValid()) {
		return false;
	}

	return true;
}

bool PointPose2H::shouldDisengage(const Frame& frame)
{
	if (Pose2H::shouldDisengage(frame)) {
		return true;
	}

	left_pointer_prev_ = left_pointer_;
	left_pointer_ = frame.finger(left_pointer_.id());
	if (!left_pointer_.isValid() || !left_pointer_.isExtended()) {
		return true;
	}

	if (left().fingers().extended().count() > 2) {
		return true;
	}

	right_pointer_prev_ = right_pointer_;
	right_pointer_ = frame.finger(right_pointer_.id());
	if (!right_pointer_.isValid() || !right_pointer_.isExtended()) {
		return true;
	}

	if (right().fingers().extended().count() > 2) {
		return true;
	}

	return false;
}

void PointPose2H::engage(const Frame& frame)
{
	Pose2H::engage(frame);
	left_pointer_prev_ = left_pointer_;
	left_pointer_engaged_ = left_pointer_;
	right_pointer_prev_ = right_pointer_;
	right_pointer_engaged_ = right_pointer_;
}

Vector PointPose2H::fingerCenter(bool stabilized) const
{
	if (stabilized) {
		return (left_pointer_.stabilizedTipPosition() + right_pointer_.stabilizedTipPosition()) * 0.5f;
	}
	return (left_pointer_.tipPosition() + right_pointer_.tipPosition()) * 0.5f;
}

Vector PointPose2H::fingerCenterEngaged(bool stabilized) const
{
	if (stabilized) {
		return (left_pointer_engaged_.stabilizedTipPosition() + right_pointer_engaged_.stabilizedTipPosition()) * 0.5f;
	}
	return (left_pointer_engaged_.tipPosition() + right_pointer_engaged_.tipPosition()) * 0.5f;
}

Vector PointPose2H::fingerCenterPrevious(bool stabilized) const
{
	if (stabilized) {
		return (left_pointer_prev_.stabilizedTipPosition() + right_pointer_prev_.stabilizedTipPosition()) * 0.5f;
	}
	return (left_pointer_prev_.tipPosition() + right_pointer_prev_.tipPosition()) * 0.5f;
}

Vector PointPose2H::fingerCenterDeltaEngaged(bool stabilized) const
{
	return fingerCenter(stabilized) - fingerCenterEngaged(stabilized);
}

Vector PointPose2H::fingerCenterDelta(bool stabilized) const
{
	return fingerCenter(stabilized) - fingerCenterPrevious(stabilized);
}