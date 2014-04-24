#ifndef __MEDLEAP_LEAP_TRACKER_H__
#define __MEDLEAP_LEAP_TRACKER_H__

#include "Leap.h"
#include <functional>
#include <chrono>

/**
Abstract class that stores tracking state for a Leap device. This class will stream updates (track) when it has been engaged.
The implementing base class determines the conditions that engage and disengage tracking.
*/
class LeapTracker
{
public:
	LeapTracker();
	virtual ~LeapTracker() {}

	/** Call each frame to update tracking status */
	void update(const Leap::Controller& controller);

	/** Tracking is active */
	bool tracking() const { return tracking_; }

	/** Manually toggles tracking */
	void tracking(bool tracking) { tracking_ = tracking; total_elapsed_ = std::chrono::milliseconds(0); }

	/** Set a callback function that is called each update tracking is enabled */
	void trackFunction(std::function<void(const Leap::Controller&)> f) { track_function_ = f; }

	/** Set a callback function that is called when tracking is turned on */
	void engageFunction(std::function<void(const Leap::Controller&)> f) { engage_function_ = f; }

	/** Set a callback function that is called when tracking is turned off */
	void disengageFunction(std::function<void(const Leap::Controller&)> f) { disengage_function_ = f; }

	/** Time that must elapse since previous disengage before an engage can occur */
	void engageDelay(std::chrono::milliseconds delay) { engage_delay_ = delay; }

	/** Time that must elapse since previous engage before a disengage can occur */
	void disengageDelay(std::chrono::milliseconds delay) { disengage_delay_ = delay; }

protected:
	virtual bool shouldEngage(const Leap::Controller& controller) = 0;
	virtual bool shouldDisengage(const Leap::Controller& controller) = 0;
	virtual void engage(const Leap::Controller& controller);
	virtual void disengage(const Leap::Controller& controller);
	virtual void track(const Leap::Controller& controller);

private:
	bool tracking_;
	std::chrono::milliseconds engage_delay_;
	std::chrono::milliseconds disengage_delay_;
	std::chrono::milliseconds total_elapsed_;
	std::chrono::system_clock::time_point last_update_;
	std::function<void(const Leap::Controller&)> track_function_;
	std::function<void(const Leap::Controller&)> engage_function_;
	std::function<void(const Leap::Controller&)> disengage_function_;
};

#endif