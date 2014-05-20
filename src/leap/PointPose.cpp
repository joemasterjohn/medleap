#include "PointPose.h"

using namespace Leap;

PointPose::PointPose(Pose1H::TrackedHand tracked) : Pose1H(tracked)
{
}

bool PointPose::shouldEngage(const Leap::Frame& frame)
{
	if (!Pose1H::shouldEngage(frame)) {
		return false;
	}

	FingerList extended = hand().fingers().extended();

	if (extended.count() > 1) {
		return false;
	}

	pointer_ = hand().fingers().frontmost();
	if (!pointer_.isExtended() || !pointer_.isValid()) {
		return false;
	}

	return true;
}

bool PointPose::shouldDisengage(const Leap::Frame& frame)
{
	if (Pose1H::shouldDisengage(frame)) {
		return true;
	}

	pointer_ = frame.finger(pointer_.id());
	if (!pointer_.isValid()) {
		return true;
	}

	if (hand().fingers().extended().count() != 1) {
		return true;
	}

	return false;
}

void PointPose::engage(const Frame& frame)
{
	Pose1H::engage(frame);
	pointer_engaged_ = pointer_;
}

void PointPose::track(const Leap::Frame& frame)
{
}