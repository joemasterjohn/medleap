#ifndef __LEAP_POSES_V_H__
#define __LEAP_POSES_V_H__

#include "Pose1H.h"

/** Index and middle finger open/close pose. */
class VPose  : public Pose1H
{
public:
	VPose();

	/** Fingers are together */
	bool isClosed() const { return closed_; }

	/** Current index finger state */
	Leap::Finger index() const { return hand().fingers()[Leap::Finger::TYPE_INDEX]; }

	/** Index finger state when engaged */
	Leap::Finger indexEngaged() const { return handEngaged().fingers()[Leap::Finger::TYPE_INDEX]; }

	/** Current middle finger state */
	Leap::Finger middle() const { return hand().fingers()[Leap::Finger::TYPE_MIDDLE]; }

	/** Middle finger state when engaged */
	Leap::Finger middleEngaged() const  { return handEngaged().fingers()[Leap::Finger::TYPE_MIDDLE]; }

	Leap::Hand handClosed() const { return hand_closed_; }

	/** Millimeters between index and middle fingers above which fingers are considered open. */
	void maxSeparation(float separation) { max_separation_ = separation; }

	/** Callback for when fingers separate */
	void openFn(std::function<void(const Leap::Frame&)> fn) { open_fn_ = fn; }

	/** Callback for when fingers close */
	void closeFn(std::function<void(const Leap::Frame&)> fn) { close_fn_ = fn; }

	float separation() const { return separation_; }

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
	bool closed_;
	float max_separation_;
	float separation_;
	Leap::Hand hand_closed_;
	std::function<void(const Leap::Frame&)> open_fn_;
	std::function<void(const Leap::Frame&)> close_fn_;
};

#endif