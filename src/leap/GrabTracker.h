#ifndef __MEDLEAP_GRAB_TRACKER_H__
#define __MEDLEAP_GRAB_TRACKER_H__

#include "LeapTracker.h"

/** Tracks a grabbing motion with one hand. */
class GrabTracker  : public LeapTracker
{
public:
	GrabTracker();

	/** State of the hand when engaged */
	Leap::Hand handEngaged() const;

	/** Current state of the hand */
	Leap::Hand handCurrent() const;

	/** Palm position when engaged */
	Leap::Vector palmPosEngaged() const;

	/** Current palm position */
	Leap::Vector palmPosCurrent() const;

	/** Vector between engaged palm position and current palm position */
	Leap::Vector palmPosDelta() const;

	/** Greatest palm speed that is allowed for tracking to engage */
	void maxEngageSpeed(float speed);

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;
	void track(const Leap::Controller& controller) override;

private:
	Leap::Hand hand_current_;
	Leap::Hand hand_engaged_;
	float max_engage_spd_;
};

#endif