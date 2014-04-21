#include "LeapTracker.h"

LeapTracker::LeapTracker() : 
		m_frames_since_change(0), 
		m_frame_threshold(25), 
		m_tracking(false)
{
}

void LeapTracker::update(const Leap::Controller& controller)
{
	if (m_tracking) {
		// if disengage tracking conditions met: change state and trigger disengage function
		if (m_frames_since_change >= m_frame_threshold && shouldDisengage(controller)) {
			m_tracking = false;
			m_frames_since_change = 0;
			disengage(controller);
		}
	}
	else {
		// if engage tracking conditions met: change state and trigger engage function
		if (m_frames_since_change >= m_frame_threshold && shouldEngage(controller)) {
			m_tracking = true;
			m_frames_since_change = 0;
			engage(controller);
		}
	}

	// call tracking function while tracking is on
	if (m_tracking) {
		track(controller);
	}

	// update time since last tracking state change
	if (m_frames_since_change < m_frame_threshold) {
		m_frames_since_change++;
	}
}