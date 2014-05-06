#ifndef __MEDLEAP_TRANSFORM_CONTROL_H__
#define __MEDLEAP_TRANSFORM_CONTROL_H__

#include "LeapTracker.h"

class TransformControl  : public LeapTracker
{
public:
	TransformControl();

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;
	void track(const Leap::Controller& controller) override;

private:

};

#endif