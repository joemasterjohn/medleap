#ifndef __MEDLEAP_CUT_TRACKER_H__
#define __MEDLEAP_CUT_TRACKER_H__

#include "PoseTracker.h"
#include <vector>

/** Index and middle finger open/close pose. */
class VPose  : public PoseTracker
{
public:
	enum class State
	{
		open,
		closed
	};

	VPose();

	/** Pose state */
	State state() const { return state_; }

	/** Current hand state */
	Leap::Hand hand() const { return hand_current_; }

	/** Hand state when engaged */
	Leap::Hand handEngaged() const { return hand_engaged_; }

	/** Current index finger state */
	Leap::Finger index() const { return hand_current_.fingers()[Leap::Finger::TYPE_INDEX]; }

	/** Index finger state when engaged */
	Leap::Finger indexEngaged() const { return hand_engaged_.fingers()[Leap::Finger::TYPE_INDEX]; }

	/** Current middle finger state */
	Leap::Finger middle() const { return hand_current_.fingers()[Leap::Finger::TYPE_MIDDLE]; }

	/** Middle finger state when engaged */
	Leap::Finger middleEngaged() const  { return hand_engaged_.fingers()[Leap::Finger::TYPE_MIDDLE]; }

	/** Set greatest palm speed that is allowed for tracking to engage */
	void maxEngageSpeed(float speed) { max_engage_spd_ = speed; }

	/** Set millimeters between index and middle fingers above which fingers are considered open. */
	void maxSeparation(float separation) { max_separation_ = separation; }

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
};

#endif