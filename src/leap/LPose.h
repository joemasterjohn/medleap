#ifndef __MEDLEAP_HAND_TRIGGER_TRACKER_H__
#define __MEDLEAP_HAND_TRIGGER_TRACKER_H__

#include "PoseTracker.h"

/** Leap tracker that continuously tracks a user's index finger and thumb.
    Tracking engages when only thumb and index are extended.
	Tracking disengages when more digits are extended or no fingers are extended.
	Open state means index and thumb make an "L" shape (close to perpendicular).
	Closed state means index and thumb are close to parallel.
*/
class LPose : public PoseTracker
{
public:
	enum class State { open, closed };

	LPose();
	State state() const { return state_; }
	const Leap::Hand& hand() const { return hand_current_; }
	const Leap::Finger index() const { return hand_current_.fingers()[Leap::Finger::TYPE_INDEX]; }
	const Leap::Finger thumb() const { return hand_current_.fingers()[Leap::Finger::TYPE_THUMB]; }
	const Leap::Hand& handEngaged() const { return hand_engaged_; }
	void openFunction(std::function<void(const Leap::Controller&)> fn) { open_function_ = fn; }
	void closeFunction(std::function<void(const Leap::Controller&)> fn) { close_function_ = fn; }

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;
	void track(const Leap::Controller& controller) override;

private:
	State state_;
	Leap::Hand hand_engaged_;
	Leap::Hand hand_current_;
	std::function<void(const Leap::Controller&)> open_function_;
	std::function<void(const Leap::Controller&)> close_function_;
};

#endif