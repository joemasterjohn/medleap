#ifndef __MEDLEAP_GRAB_TRACKER_H__
#define __MEDLEAP_GRAB_TRACKER_H__

#include "PoseTracker.h"

/** Tracks a grabbing motion with one hand. */
class FistPose  : public PoseTracker
{
public:
	FistPose();

	/** Fingers are spread out */
	bool isOpen() const { return open_; }

	/** Current state of the hand */
	Leap::Hand hand() const { return hand_current_; }

	/** State of the hand when engaged */
	Leap::Hand handEngaged() const { return hand_engaged_; }

	/** State of the hand from previous frame */
	Leap::Hand handPrevious() const { return hand_previous_; }

	/** Greatest palm speed that is allowed for tracking to engage */
	void maxEngageSpeed(float speed);

	unsigned framesTracked() const { return frames_tracked_; }

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;
	void track(const Leap::Controller& controller) override;

private:
	Leap::Hand hand_current_;
	Leap::Hand hand_engaged_;
	Leap::Hand hand_previous_;
	float max_engage_spd_;
	bool open_;
	unsigned frames_tracked_;
};

#endif