#ifndef __LEAP_POSES_POSE1_H__
#define __LEAP_POSES_POSE1_H__

#include "Pose.h"

/** Tracks a specific hand */
class Pose1H : public Pose
{
public:
	/** Determines which hand tracking will engage on */
	enum class TrackedHand {
		/** Engage on left hand */
		left, 
		/** Engage on right hand */
		right, 
		/** Engage on frontmost hand */
		front,
		/** Engage on the first hand to enter Leap's FOV */
		first,
		/** Engage on the second hand to enter Leap's FOV */
		second
	};

	Pose1H(TrackedHand tracked = TrackedHand::first);

	virtual ~Pose1H();

	/** Current hand state */
	const Leap::Hand& hand() const { return hand_; }

	/** Previous hand state */
	const Leap::Hand& handPrevious() const { return hand_previous_; }

	/** Hand state when engaged */
	const Leap::Hand& handEngaged() const { return hand_engaged_; }

	/** Maximum speed hand can be moving in mm/sec to engage. Use speed <= 0 for any speed. */
	void maxHandEngageSpeed(float speed) { max_hand_engage_speed_ = speed; }

protected:
	virtual bool shouldEngage(const Leap::Frame& frame) override;
	virtual bool shouldDisengage(const Leap::Frame& frame) override;
	virtual void engage(const Leap::Frame& frame) override;
	virtual void disengage(const Leap::Frame& frame) override;
	virtual void track(const Leap::Frame& frame) override;

private:
	TrackedHand tracked_;
	Leap::Hand hand_;
	Leap::Hand hand_previous_;
	Leap::Hand hand_engaged_;
	float max_hand_engage_speed_;
};

#endif