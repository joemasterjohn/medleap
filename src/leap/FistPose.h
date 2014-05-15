#ifndef __LEAP_POSES_FIST_H__
#define __LEAP_POSES_FIST_H__

#include "Pose1H.h"

class FistPose  : public Pose1H
{
public:
	enum class State
	{
		/** All digits are extended */
		open,
		/** Some digits are extended */
		partial,
		/** No digits are extended */
		closed
	};

	FistPose();

	/** Current state */
	State state() { return state_; }

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
	State state_;
};

#endif