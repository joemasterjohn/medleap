#ifndef __MEDLEAP_HAND_TRIGGER_TRACKER_H__
#define __MEDLEAP_HAND_TRIGGER_TRACKER_H__

#include "LeapTracker.h"
#include <functional>

/** Leap tracker that continuously tracks a user's index finger and thumb. Tracking state is toggled by the user "clicking" their thumb. */
class HandTriggerTracker  : public LeapTracker
{
public:
	HandTriggerTracker();

	// Tip position of the index finger when tracking was engaged
	Leap::Vector engagedTipPos() const { return m_engage_tip_pos; }

	// Difference of current index finger tip position and where it was when engaged
	Leap::Vector deltaTipPos() const { return m_delta_tip_pos; }

protected:
	bool shouldEngage(const Leap::Controller& controller) override;
	bool shouldDisengage(const Leap::Controller& controller) override;
	void engage(const Leap::Controller& controller) override;
	void track(const Leap::Controller& controller) override;

private:
	float m_thumb_spd_thresh;
	float m_index_spd_thresh;
	float m_delta_spd_thresh;
	Leap::Vector m_engage_tip_pos;
	Leap::Vector m_delta_tip_pos;
};

#endif