#include "TwoFingerTracker.h"

using namespace Leap;

TwoFingerTracker::TwoFingerTracker() :
		engage_spd_thresh_(50),
		disengage_spd_thresh_(250)
{
	engageDelay(std::chrono::milliseconds(1000));
}

bool TwoFingerTracker::shouldEngage(const Leap::Controller& controller)
{
	HandList hands = controller.frame().hands();

	if (hands.count() != 2)
		return false;

	if (hands[0].fingers().count() > 2 || hands[1].fingers().count() > 2)
		return false;

	l_index_ = hands.leftmost().fingers().rightmost();
	r_index_ = hands.rightmost().fingers().leftmost();

	if (l_index_.tipVelocity().magnitude() > engage_spd_thresh_)
		return false;

	if (r_index_.tipVelocity().magnitude() > engage_spd_thresh_)
		return false;

	return true;
}

bool TwoFingerTracker::shouldDisengage(const Leap::Controller& controller)
{
	HandList hands = controller.frame().hands();

	if (hands.count() != 2)
		return true;

	if (hands[0].fingers().isEmpty() || hands[1].fingers().isEmpty())
		return true;

	// maybe better to use velocity perp. to hand axis, not to camera
	if (leftIndex(controller.frame()).tipVelocity().z > disengage_spd_thresh_)
		return true;

	if (rightIndex(controller.frame()).tipVelocity().z > disengage_spd_thresh_)
		return true;

	return false;
}

Finger TwoFingerTracker::leftIndex() const
{
	return l_index_;
}

Finger TwoFingerTracker::leftIndex(const Frame& frame) const
{
	return frame.finger(l_index_.id());
}

Finger TwoFingerTracker::rightIndex() const
{
	return r_index_;
}

Finger TwoFingerTracker::rightIndex(const Frame& frame) const
{
	return frame.finger(r_index_.id());
}

float TwoFingerTracker::fingerGap() const
{
	Vector l = l_index_.tipPosition();
	Vector r = r_index_.tipPosition();
	return (l - r).magnitude();
}

float TwoFingerTracker::fingerGap(const Leap::Frame& frame) const
{
	Vector l = leftIndex(frame).tipPosition();
	Vector r = rightIndex(frame).tipPosition();
	return (l - r).magnitude();
}

float TwoFingerTracker::fingerGapDelta(const Leap::Frame& frame) const
{
	return fingerGap(frame) - fingerGap();
}

Vector TwoFingerTracker::centerPos() const
{
	Vector l = l_index_.tipPosition();
	Vector r = r_index_.tipPosition();
	return r + (l - r) / 2.0f;
}

Vector TwoFingerTracker::centerPos(const Frame& frame) const
{
	Vector l = leftIndex(frame).tipPosition();
	Vector r = rightIndex(frame).tipPosition();
	return r + (l - r) / 2.0f;
}

Vector TwoFingerTracker::centerPosDelta(const Leap::Frame& frame) const
{
	return centerPos(frame) - centerPos();
}

Vector TwoFingerTracker::leftIndexPosDelta(const Frame& frame) const
{
	return frame.finger(l_index_.id()).tipPosition() - l_index_.tipPosition();
}

Vector TwoFingerTracker::rightIndexPosDelta(const Frame& frame) const
{
	return frame.finger(r_index_.id()).tipPosition() - r_index_.tipPosition();
}

void TwoFingerTracker::engageSpeedThreshold(float speed)
{
	engage_spd_thresh_ = speed;
}

void TwoFingerTracker::disengageSpeedThreshold(float speed)
{
	disengage_spd_thresh_ = speed;
}