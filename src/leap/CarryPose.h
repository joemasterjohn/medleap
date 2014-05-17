#ifndef __LEAP_POSES_CARRY_H__
#define __LEAP_POSES_CARRY_H__

#include "Pose1H.h"

class CarryPose  : public Pose1H
{
public:
	CarryPose();

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;

private:
};

#endif