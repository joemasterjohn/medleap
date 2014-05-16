#ifndef __LEAP_POSES_CARRY_H__
#define __LEAP_POSES_CARRY_H__

#include "Pose2H.h"

class CarryPose  : public Pose2H
{
public:
	CarryPose();

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
};

#endif