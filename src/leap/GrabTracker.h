#ifndef __MEDLEAP_GRAB_TRACKER_H__
#define __MEDLEAP_GRAB_TRACKER_H__

#include "LeapTracker.h"

/** Leap tracker that continuously tracks two index fingers from both hands. */
class GrabTracker  : public LeapTracker
{
public:
	GrabTracker();

	/** The grabbing hand */
	Leap::Hand hand() const;

	/** Position when tracking engaged */
	Leap::Vector palmPos() const;

	/** Position for given frame */
	Leap::Vector palmPos(const Leap::Frame& frame) const;

	/** Change in position from when tracking engaged */
	Leap::Vector palmPosDelta(const Leap::Frame& frame) const;

	/** Max speed fingers can be moving and have tracking engage */
	void engageSpeedThreshold(float speed);

	/** Max speed fingers can move before tracking will disengage */
	void disengageSpeedThreshold(float speed);

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;

private:
	Leap::Hand hand_;
	float engage_spd_thresh_;
	float disengage_spd_thresh_;
};

#endif