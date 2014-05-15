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

protected:
	virtual bool shouldEngage(const Leap::Frame& frame) override;
	virtual bool shouldDisengage(const Leap::Frame& frame) override;
	virtual void engage(const Leap::Frame& frame) override;
	virtual void disengage(const Leap::Frame& frame) override;
	virtual void track(const Leap::Frame& frame) override;

private:
	Leap::Hand left_;
	Leap::Hand left_previous_;
	Leap::Hand left_engaged_;
	Leap::Hand right_;
	Leap::Hand right_previous_;
	Leap::Hand right_engaged_;
	float max_hand_engage_speed_;
};

#endif