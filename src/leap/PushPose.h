#ifndef __LEAP_POSES_PUSH_H__
#define __LEAP_POSES_PUSH_H__

#include "Pose1H.h"

class PushPose  : public Pose1H
{
public:
	PushPose();

	bool isClosed() const { return closed_; }

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
	bool closed_;
};

#endif