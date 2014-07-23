#include "LPose.h"

using namespace Leap;
using namespace std::chrono;

LPose::LPose() :
	closed_(false),
	last_close_(high_resolution_clock::now()),
	open_fn_(nullptr),
	close_fn_(nullptr),
    close_separation_(35.0f)
{
	maxHandEngageSpeed(155.0f);
}

bool LPose::shouldEngage(const Frame& frame)
{
	if (!Pose1H::shouldEngage(frame)) {
		return false;
	}

	FingerList fingers = hand().fingers();

	if (!fingers[Finger::TYPE_THUMB].isExtended() ||
		!fingers[Finger::TYPE_INDEX].isExtended() ||
		fingers[Finger::TYPE_MIDDLE].isExtended() ||
		fingers[Finger::TYPE_RING].isExtended() ||
		fingers[Finger::TYPE_PINKY].isExtended())
	{
		return false;
	}
    
	Finger thumb = fingers[Finger::TYPE_THUMB];
	Finger index = fingers[Finger::TYPE_INDEX];

    Vector u = index.bone(Bone::TYPE_PROXIMAL).prevJoint();
    Vector v = thumb.tipPosition();
    float d = (u-v).magnitude();
	if (d < close_separation_) {
		return false;
	}

	return true;
}

bool LPose::shouldDisengage(const Frame& frame)
{
	if (Pose1H::shouldDisengage(frame)) {
		return true;
	}

	FingerList fingers = hand().fingers();
    
	if (!fingers[Finger::TYPE_THUMB].isExtended() ||
		!fingers[Finger::TYPE_INDEX].isExtended() ||
		fingers[Finger::TYPE_MIDDLE].isExtended() ||
		fingers[Finger::TYPE_RING].isExtended() ||
		fingers[Finger::TYPE_PINKY].isExtended())
	{
		return true;
	}

	return false;
}

void LPose::track(const Frame& frame)
{
    Vector u = hand().fingers()[Finger::TYPE_INDEX].bone(Bone::TYPE_PROXIMAL).prevJoint();
    Vector v = hand().fingers()[Finger::TYPE_THUMB].tipPosition();
    separation_ = (u-v).magnitude();

	bool was_closed = closed_;
	closed_ = separation_ <= close_separation_;

	if (hand().confidence() > 0.75f && was_closed && !closed_) {
		if (open_fn_) {
			open_fn_(frame);
		}
		if (click_fn_) {
			auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - last_close_);
			if (elapsed.count() < 200.0f) {
				click_fn_(frame);
			}
		}
	} else if (hand().confidence() > 0.75f && !was_closed && closed_) {
		if (close_fn_) {
			close_fn_(frame);
		}
        hand_closed_ = hand();
		last_close_ = high_resolution_clock::now();
	}
}

Vector LPose::pointerDelta(bool stabilized) const
{
	if (stabilized) {
		return hand().fingers()[Finger::TYPE_INDEX].stabilizedTipPosition() - handPrevious().fingers()[Finger::TYPE_INDEX].stabilizedTipPosition();
	}
	return hand().fingers()[Finger::TYPE_INDEX].tipPosition() - handPrevious().fingers()[Finger::TYPE_INDEX].tipPosition();
}