#ifndef __MEDLEAP_ONE_FINGER_TRACKER_H__
#define __MEDLEAP_ONE_FINGER_TRACKER_H__

#include "LeapTracker.h"
#include <functional>

/** Leap tracker that continuously tracks one index finger. */
class OneFingerTracker  : public LeapTracker
{
public:
	OneFingerTracker();

	/** The state of the finger when tracking engaged */
	Leap::Finger finger() const;

	/** The state of the finger at the specified frame */
	Leap::Finger finger(const Leap::Frame& frame) const;

	/** Difference of current finger position and position when tracking engaged */
	Leap::Vector posDelta(const Leap::Frame& frame) const;

	/** Max speed fingers can be moving and have tracking engage */
	void engageSpeedThreshold(float speed) { engage_spd_thresh_ = speed; }

	/** Max speed fingers can move before tracking will disengage */
	void disengageSpeedThreshold(float speed) { disengage_spd_thresh_ = speed; }

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;

private:
	Leap::Finger finger_;
	float engage_spd_thresh_;
	float disengage_spd_thresh_;
};

#endif