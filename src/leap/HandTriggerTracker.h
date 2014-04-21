#ifndef __MEDLEAP_HAND_TRIGGER_TRACKER_H__
#define __MEDLEAP_HAND_TRIGGER_TRACKER_H__

#include "LeapTracker.h"
#include <functional>

/** Leap tracker that continuously tracks a user's index finger and thumb. Tracking state is toggled by the user "clicking" their thumb. */
class HandTriggerTracker  : public LeapTracker
{
public:
	HandTriggerTracker();
	void trackFunction(std::function<void(const Leap::Controller&)> f) { m_track_function = f; }
	void engageFunction(std::function<void(const Leap::Controller&)> f) { m_engage_function = f; }
	void disengageFunction(std::function<void(const Leap::Controller&)> f) { m_disengage_function = f; }

	// Tip position of the index finger when tracking was engaged
	Leap::Vector engagedTipPos() const { return m_engage_tip_pos; }

	// Difference of current index finger tip position and where it was when engaged
	Leap::Vector deltaTipPos() const { return m_delta_tip_pos; }

protected:
	bool shouldEngage(const Leap::Controller& controller);
	bool shouldDisengage(const Leap::Controller& controller);
	void engage(const Leap::Controller& controller);
	void disengage(const Leap::Controller& controller);
	void track(const Leap::Controller& controller);

private:
	std::function<void(const Leap::Controller&)> m_track_function;
	std::function<void(const Leap::Controller&)> m_engage_function;
	std::function<void(const Leap::Controller&)> m_disengage_function;
	float m_thumb_spd_thresh;
	float m_index_spd_thresh;
	float m_delta_spd_thresh;
	Leap::Vector m_engage_tip_pos;
	Leap::Vector m_delta_tip_pos;
};

#endif