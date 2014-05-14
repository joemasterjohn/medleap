#ifndef __MEDLEAP_PINCH_POSE_H__
#define __MEDLEAP_PINCH_POSE_H__

#include "PoseTracker.h"

class PinchPose : public PoseTracker
{
public:
	enum class State
	{
		pinch_left,
		pinch_right,
		pinch_both,
		open
	};

	PinchPose();
	State state() const { return state_; }
	const Leap::Hand& left() const { return left_; }
	const Leap::Hand& leftPinched() const { return left_pinched_; }
	const Leap::Hand& right() const { return right_; }
	const Leap::Hand& rightPinched() const { return right_pinched_; }

	/** Stabilized hand position. Uses whichever hand is available; if both hands are present it is the average. */
	Leap::Vector positionStabilized() const;

	void stateFunction(std::function<void(const Leap::Controller&, State prev, State curr)> fn) { state_fn_ = fn; }
	void doublePinchFunction(std::function<void(const Leap::Controller&)> fn) { dbl_pinch_fn_ = fn; }
protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;
	void track(const Leap::Controller& controller) override;

private:
	State state_;
	Leap::Hand left_;
	Leap::Hand left_pinched_;
	Leap::Hand right_;
	Leap::Hand right_pinched_;
	std::chrono::high_resolution_clock::time_point last_pinch_;
	std::function<void(const Leap::Controller&, State prev, State curr)> state_fn_;
	std::function<void(const Leap::Controller&)> dbl_pinch_fn_;
};

#endif