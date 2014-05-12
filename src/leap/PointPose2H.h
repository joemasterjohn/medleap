#ifndef __MEDLEAP_TWO_FINGER_TRACKER_H__
#define __MEDLEAP_TWO_FINGER_TRACKER_H__

#include "PoseTracker.h"
#include <functional>

/** Leap tracker that continuously tracks two index fingers from both hands. */
class PointPose2H  : public PoseTracker
{
public:
	PointPose2H();

	/** Current state of left hand */
	Leap::Hand leftHand() const { return left_current_; }

	/** State of left hand when engaged */
	Leap::Hand leftHandEngaged() const { return left_engaged_; }

	/** Current state of right hand */
	Leap::Hand rightHand() const { return right_current_; }

	/** State of right hand when engaged */
	Leap::Hand rightHandEngaged() const { return right_engaged_; }

	/** Current state of left index finger */
	Leap::Finger leftIndex() const { return left_current_.fingers()[Leap::Finger::TYPE_INDEX]; }

	/** State of left index finger when engaged */
	Leap::Finger leftIndexEngaged() const { return left_engaged_.fingers()[Leap::Finger::TYPE_INDEX]; }

	/** Current state of right index finger */
	Leap::Finger rightIndex() const { return right_current_.fingers()[Leap::Finger::TYPE_INDEX]; }

	/** State of right index finger when engaged */
	Leap::Finger rightIndexEngaged() const { return right_engaged_.fingers()[Leap::Finger::TYPE_INDEX]; }

	/** Distance between left and right index fingers */
	float fingerGap() const;

	/** Distance between left and right index fingers when engaged */
	float fingerGapEngaged() const;

	/** Change in distance between left and right index fingers from frame when engaged */
	float fingerGapDelta() const;

	/** Position between left and right index fingers */
	Leap::Vector center() const;

	/** Position between left and right index fingers when tracking engaged */
	Leap::Vector centerEngaged() const;

	/** Change in position between left and right index fingers from frame when engaged */
	Leap::Vector centerDelta() const;

	/** Max speed fingers can be moving and have tracking engage */
	void engageSpeedThreshold(float speed) { engage_spd_thresh_ = speed; }

	/** Max speed fingers can move before tracking will disengage */
	void disengageSpeedThreshold(float speed) { disengage_spd_thresh_ = speed; }

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;
	void track(const Leap::Controller& controller);

private:
	Leap::Hand left_engaged_;
	Leap::Hand left_current_;
	Leap::Hand right_engaged_;
	Leap::Hand right_current_;
	float engage_spd_thresh_;
	float disengage_spd_thresh_;
};

#endif