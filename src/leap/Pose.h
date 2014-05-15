#ifndef __LEAP_POSES_POSE_H__
#define __LEAP_POSES_POSE_H__

#include "Leap.h"
#include <functional>
#include <chrono>

/**
Abstract class that stores tracking state for a Leap device. This class will stream updates (track) when it has been engaged.
The implementing base class determines the conditions that engage and disengage tracking. Optional callbacks can be registered
to receive notifications when specific events occur.
*/
class Pose
{
public:
	Pose();

	virtual ~Pose();

	/** Call each frame to update tracking status */
	void update(const Leap::Frame& frame);

	/** Tracking is active */
	bool tracking() const { return tracking_; }

	/** Manually toggles tracking */
	void tracking(bool tracking);

	/** Number of frames tracking has been active */
	uint64_t frames() const { return frames_tracked_; }

	/** Set a callback function that is called each update tracking is enabled */
	void trackFunction(std::function<void(const Leap::Frame& frame)> f) { track_function_ = f; }

	/** Set a callback function that is called when tracking is turned on */
	void engageFunction(std::function<void(const Leap::Frame&)> f) { engage_function_ = f; }

	/** Set a callback function that is called when tracking is turned off */
	void disengageFunction(std::function<void(const Leap::Frame&)> f) { disengage_function_ = f; }

	/** Time that must elapse since previous disengage before an engage can occur */
	void engageDelay(std::chrono::milliseconds delay) { engage_delay_ = delay; }

	/** Time that must elapse since previous engage before a disengage can occur */
	void disengageDelay(std::chrono::milliseconds delay) { disengage_delay_ = delay; }

	/** Minimum number of frames where shouldEngage evaluates to true before tracking engages */
	void minValidFrames(uint64_t frames) { min_valid_frames_ = frames; }

protected:
	/** Returns true if pose should start tracking */
	virtual bool shouldEngage(const Leap::Frame& frame) = 0;

	/** Returns true if pose should stop tracking */
	virtual bool shouldDisengage(const Leap::Frame& frame) = 0;

	/** Called when tracking starts */
	virtual void engage(const Leap::Frame& frame);

	/** Called when tracking stops */
	virtual void disengage(const Leap::Frame& frame);

	/** Called each update if tracking is active */
	virtual void track(const Leap::Frame& frame);

private:
	bool tracking_;
	uint64_t min_valid_frames_;
	uint64_t valid_frames_;
	uint64_t frames_tracked_;
	std::chrono::milliseconds engage_delay_;
	std::chrono::milliseconds disengage_delay_;
	std::chrono::milliseconds total_elapsed_;
	std::chrono::high_resolution_clock::time_point last_update_;
	std::function<void(const Leap::Frame&)> track_function_;
	std::function<void(const Leap::Frame&)> engage_function_;
	std::function<void(const Leap::Frame&)> disengage_function_;
};

#endif