#ifndef __LEAP_POSES_L_H__
#define __LEAP_POSES_L_H__

#include "Pose1H.h"

/** Leap tracker that continuously tracks a user's index finger and thumb.
    Tracking engages when only thumb and index are extended.
	Tracking disengages when more digits are extended or no fingers are extended.
	Open state means index and thumb make an "L" shape (close to perpendicular).
	Closed state means index and thumb are close to parallel.
*/
class LPose : public Pose1H
{
public:
	LPose();

	/** Pointer and thumb are together */
	bool isClosed() const { return closed_; }

	/** Current state of pointer finger (index or middle possibly) */
	const Leap::Finger& pointer() const { return pointer_; }

	/** Current state of thumb */
	const Leap::Finger& thumb() const { return thumb_; }

	/** Callback for when fingers separate */
	void openFn(std::function<void(const Leap::Frame&)> fn) { open_fn_ = fn; }

	/** Callback for when fingers close */
	void closeFn(std::function<void(const Leap::Frame&)> fn) { close_fn_ = fn; }

	void clickFn(std::function <void(const Leap::Frame&)> fn) { click_fn_ = fn; }

protected:
	bool shouldEngage(const Leap::Frame& frame) override;
	bool shouldDisengage(const Leap::Frame& frame) override;
	void track(const Leap::Frame& frame) override;

private:
	bool closed_;
	Leap::Finger pointer_;
	Leap::Finger thumb_;
	std::chrono::high_resolution_clock::time_point last_close_;
	std::function<void(const Leap::Frame&)> open_fn_;
	std::function<void(const Leap::Frame&)> close_fn_;
	std::function<void(const Leap::Frame&)> click_fn_;
};

#endif