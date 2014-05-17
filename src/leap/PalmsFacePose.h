#ifndef __LEAP_POSES_PALMS_FACE_H__
#define __LEAP_POSES_PALMS_FACE_H__

#include "Pose2H.h"

class PalmsFacePose  : public Pose2H
{
public:
	PalmsFacePose();

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
};

#endif