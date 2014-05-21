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
		closed,
		/** Fingers curled in, thumb out */
		thumb_out,
		/** Thumb, index, middle extended */
		three_out,
	};

	FistPose();

	/** Current state */
	State state() { return state_; }

	std::chrono::milliseconds timeSinceStateChange() { return time_since_change_; }

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
	State state_;
	std::chrono::high_resolution_clock::time_point last_change_;
	std::chrono::milliseconds time_since_change_;
};

#endif