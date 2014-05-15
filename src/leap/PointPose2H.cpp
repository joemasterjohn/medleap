#include "PointPose2H.h"

using namespace Leap;

PointPose2H::PointPose2H()
{
	minValidFrames(5);
	maxHandEngageSpeed(100.0f);
}

bool PointPose2H::shouldEngage(const Frame& frame)
{
	if (!Pose2H::shouldEngage(frame)) {
		return false;
	}

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
	if (!left_pointer_.isValid()) {
		return true;
	}

	if (left().fingers().extended().count() > 2) {
		return true;
	}

	right_pointer_prev_ = right_pointer_;
	right_pointer_ = frame.finger(right_pointer_.id());
	if (!right_pointer_.isValid()) {
		return true;
	}

	if (right().fingers().extended().count() > 2) {
		return true;
	}

	return false;
}

void PointPose2H::engage(const Frame& frame)
{
	left_pointer_prev_ = left_pointer_;
	left_pointer_engaged_ = left_pointer_;
	right_pointer_prev_ = right_pointer_;
	right_pointer_engaged_ = right_pointer_;
}

Vector PointPose2H::center() const
{
	return (left_pointer_.tipPosition() + right_pointer_.tipPosition()) * 0.5f;
}

Vector PointPose2H::deltaCenter() const
{
	Vector curr_c = (left_pointer_.tipPosition() + right_pointer_.tipPosition()) * 0.5f;
	Vector prev_c = (left_pointer_prev_.tipPosition() + right_pointer_prev_.tipPosition()) * 0.5f;
	return curr_c - prev_c;
}

Vector PointPose2H::deltaCenterEngaged() const
{
	Vector curr_c = (left_pointer_.tipPosition() + right_pointer_.tipPosition()) * 0.5f;
	Vector engd_c = (left_pointer_engaged_.tipPosition() + right_pointer_engaged_.tipPosition()) * 0.5f;
	return curr_c - engd_c;
}