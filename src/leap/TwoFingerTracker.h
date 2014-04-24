#ifndef __MEDLEAP_TWO_FINGER_TRACKER_H__
#define __MEDLEAP_TWO_FINGER_TRACKER_H__

#include "LeapTracker.h"
#include <functional>

/** Leap tracker that continuously tracks two index fingers from both hands. */
class TwoFingerTracker  : public LeapTracker
{
public:
	TwoFingerTracker();

	/** Returns the state of the left index finger when tracking engaged */
	Leap::Finger leftIndex() const;

	/** Returns the state of the left index finger at the specified frame */
	Leap::Finger leftIndex(const Leap::Frame& frame) const;

	/** Returns the state of the right index finger when tracking engaged */
	Leap::Finger rightIndex() const;

	/** Returns the state of the right index finger at the specified frame */
	Leap::Finger rightIndex(const Leap::Frame& frame) const;

	/** Position between left and right index fingers when tracking engaged */
	Leap::Vector centerPos() const;

	/** Distance between left and right index fingers when tracking engaged */
	float fingerGap() const;

	/** Distance between left and right index fingers at specified frame */
	float fingerGap(const Leap::Frame& frame) const;

	/** Difference between current finger gap and gap when tracking was engaged */
	float fingerGapDelta(const Leap::Frame& frame) const;

	/** Position between left and right index fingers when tracking engaged */
	Leap::Vector centerPos(const Leap::Frame& frame) const;

	/** Change in position between left and right index fingers from when tracking engaged */
	Leap::Vector centerPosDelta(const Leap::Frame& frame) const;

	/** Returns difference of left index finger's tip position and where it was when engaged */
	Leap::Vector leftIndexPosDelta(const Leap::Frame& frame) const;

	/** Returns difference of right index finger's tip position and where it was when engaged */
	Leap::Vector rightIndexPosDelta(const Leap::Frame& frame) const;

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;

private:
	Leap::Finger l_index_;
	Leap::Finger r_index_;
	float engage_velocity_thresh_;
	float disengage_velocity_thresh_;
};

#endif