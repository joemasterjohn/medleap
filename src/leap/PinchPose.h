#ifndef __LEAP_POSES_PINCH_H__
#define __LEAP_POSES_PINCH_H__

#include "Pose1H.h"

class PinchPose : public Pose1H
{
public:
	/** Creates a pinch pose tracker */
	PinchPose();

	/** Currently pinching */
	bool isPinching() { return pinching_; }

	/** State of the hand when state changed from open to closed */
	const Leap::Hand& handPinched() const { return hand_pinched_; }

	/** Threshold required for state to change from open to closed */
	void pinchStrength(float strength) { pinch_strength_ = strength; }

	/** Threshold required for state to change from closed to open */
	void releaseStrength(float strength) { release_strength_ = strength; }

	/** Callback for when fingers separate */
	void openFn(std::function<void(const Leap::Frame&)> fn) { open_fn_ = fn; }

	/** Callback for when fingers close */
	void closeFn(std::function<void(const Leap::Frame&)> fn) { close_fn_ = fn; }

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
	Leap::Hand hand_pinched_;
	bool pinching_;
	float pinch_strength_;
	float release_strength_;
	std::function<void(const Leap::Frame&)> open_fn_;
	std::function<void(const Leap::Frame&)> close_fn_;
};

#endif