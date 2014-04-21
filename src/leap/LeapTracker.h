#ifndef __MEDLEAP_LEAP_TRACKER_H__
#define __MEDLEAP_LEAP_TRACKER_H__

#include "Leap.h"

/**
Abstract class that stores tracking state for a Leap device. This class will stream updates (track) when it has been engaged.
The implementing base class determines the conditions that engage and disengage tracking.
*/
class LeapTracker
{
public:
	LeapTracker();
	virtual ~LeapTracker() {}
	void update(const Leap::Controller& controller);
	bool tracking() const { return m_tracking; }

protected:
	virtual bool shouldEngage(const Leap::Controller& controller) = 0;
	virtual bool shouldDisengage(const Leap::Controller& controller) = 0;
	virtual void engage(const Leap::Controller& controller) = 0;
	virtual void disengage(const Leap::Controller& controller) = 0;
	virtual void track(const Leap::Controller& controller) = 0;

private:
	int m_frames_since_change;
	int m_frame_threshold;
	bool m_tracking;
};

#endif