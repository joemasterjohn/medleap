#ifndef __MEDLEAP_CUT_TRACKER_H__
#define __MEDLEAP_CUT_TRACKER_H__

#include "LeapTracker.h"
#include <vector>

/** Tracks a scissor open/close motion with one hand. */
class CutTracker  : public LeapTracker
{
public:
	enum class State
	{
		searching,
		hand_stopped,
		fingers_acquired,
		tracking
	};

	CutTracker();

	State state() const { return state_; }
	Leap::Hand handCurrent() { return hand_current_; }

	/** Greatest palm speed that is allowed for tracking to engage */
	void maxEngageSpeed(float speed);

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;
	void track(const Leap::Controller& controller) override;

private:
	State state_;
	float max_engage_spd_;
	float max_separation_;
	Leap::Hand hand_engaged_;
	Leap::Hand hand_current_;
	Leap::Finger closest_finger_;
	Leap::Finger second_closest_finger_;

	void checkHandStop(const Leap::Controller& controller);
	void acquireFingers(const Leap::Controller& controller);
	void fingerClose(const Leap::Controller& controller);

	std::vector<Leap::Finger> sortFingers(const Leap::Hand& hand);
};

#endif