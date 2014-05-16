#ifndef __LEAP_POSES_POSE2_H__
#define __LEAP_POSES_POSE2_H__

#include "Pose.h"

/** Pose that requires two hands */
class Pose2H : public Pose
{
public:
	Pose2H();

	virtual ~Pose2H();

	const Leap::Hand& left() const { return left_; }
	const Leap::Hand& leftPrevious() const { return left_previous_; }
	const Leap::Hand& leftEngaged() const { return left_engaged_; }
	const Leap::Hand& right() const { return right_; }
	const Leap::Hand& rightPrevious() const { return right_previous_; }
	const Leap::Hand& rightEngaged() const { return right_engaged_; }

	/** Maximum speed either hand can be moving in mm/sec to engage. Use speed <= 0 for any speed. */
	void maxHandEngageSpeed(float speed) { max_hand_engage_speed_ = speed; }

	/** Speed at which hands are considered exiting */
	void exitSpeed(float speed) { exit_speed_ = speed; }

	/** Tracking should disengage if hands are moving at or above exit speed in +Z */
	void disengageOnExit(bool disengage_on_exit) { disengage_on_exit_ = disengage_on_exit; }

	/** Average of both palm positions */
	Leap::Vector handsCenter(bool stabilized = false) const;

	/** Average of palm positions from previous frame */
	Leap::Vector handsCenterPrevious(bool stabilized = false) const;

	/** Average of palm positions when engaged */
	Leap::Vector handsCenterEngaged(bool stabilized = false) const;

	/** Difference in current hands center and hands center from previous frame */
	Leap::Vector handsCenterDelta(bool stabilized = false) const;

	/** Difference in current hands center and hands center when engaged */
	Leap::Vector handsCenterDeltaEngaged(bool stabilized = false) const;

	/** Millimeters between left and right palm positions */
	float handsSeparation(bool stabilized = false) const;

	/** Millimeters between left and right palm positions when engaged */
	float handsSeparationEngaged(bool stabilized = false) const;

	/** Millimeters between left and right palm positions from previous frame */
	float handsSeparationPrevious(bool stabilized = false) const;

	/** Difference from hand separation in previous frame */
	float handsSeparationDelta(bool stabilized = false) const;

	/** Difference from hand separation in engaged frame */
	float handsSeparationDeltaEngaged(bool stabilized = false) const;

protected:
	virtual bool shouldEngage(const Leap::Frame& frame) override;
	virtual bool shouldDisengage(const Leap::Frame& frame) override;
	virtual void engage(const Leap::Frame& frame) override;

private:
	Leap::Hand left_;
	Leap::Hand left_previous_;
	Leap::Hand left_engaged_;
	Leap::Hand right_;
	Leap::Hand right_previous_;
	Leap::Hand right_engaged_;
	float max_hand_engage_speed_;
	float exit_speed_;
	bool disengage_on_exit_;
};

#endif