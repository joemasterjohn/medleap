#ifndef __LEAP_POSES_PUSH_H__
#define __LEAP_POSES_PUSH_H__

#include "Pose1H.h"

class PushPose  : public Pose1H
{
public:
	PushPose();

	bool isClosed() const { return closed_; }

	const Leap::Hand& handClosed() const { return hand_closed_; }

	Leap::Vector handPositionClosed(bool stabilized = false) const;

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
	Leap::Hand hand_closed_;
	bool closed_;
};

#endif