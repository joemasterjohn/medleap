#ifndef __MEDLEAP_ONE_FINGER_TRACKER_H__
#define __MEDLEAP_ONE_FINGER_TRACKER_H__

#include "PoseTracker.h"
#include <functional>

/** Leap tracker that tracks a one-finger pose. */
class PointPose  : public PoseTracker
{
public:
	PointPose();

	/** Current state of index finger */
	Leap::Finger index() const { return current_; }

	/** State of index finger when engaged */
	Leap::Finger indexEngaged() const { return engaged_; }

	/** Difference of current finger position and position when tracking engaged */
	Leap::Vector posDelta() const;

	/** Max speed fingers can be moving and have tracking engage */
	void engageSpeedThreshold(float speed) { engage_spd_thresh_ = speed; }

	/** Max speed fingers can move before tracking will disengage */
	void disengageSpeedThreshold(float speed) { disengage_spd_thresh_ = speed; }

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;
	void track(const Leap::Controller& controller) override;

private:
	Leap::Finger engaged_;
	Leap::Finger current_;
	float engage_spd_thresh_;
	float disengage_spd_thresh_;
};

#endif