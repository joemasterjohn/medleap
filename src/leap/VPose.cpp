#include "VPose.h"
#include <algorithm>

using namespace Leap;
using namespace std;

VPose::VPose() : 
	closed_(false),
	max_separation_(25.0f),
	open_fn_(nullptr),
	close_fn_(nullptr)
{
	minValidFrames(5);
	maxHandEngageSpeed(150.0f);
}

bool VPose::shouldEngage(const Leap::Frame& frame)
{
	if (!Pose1H::shouldEngage(frame))
		return false;

	FingerList fingers = hand().fingers();
	if (fingers[Finger::TYPE_THUMB].isExtended() ||
		!fingers[Finger::TYPE_INDEX].isExtended() ||
		!fingers[Finger::TYPE_MIDDLE].isExtended() ||
		fingers[Finger::TYPE_RING].isExtended() ||
		fingers[Finger::TYPE_PINKY].isExtended())
	{
		return false;
	}

	closed_ = false;
	return true;
}

bool VPose::shouldDisengage(const Leap::Frame& frame)
{
	if (Pose1H::shouldDisengage(frame)) {
		return true;
	}

	FingerList fingers = hand().fingers();
	if (fingers[Finger::TYPE_THUMB].isExtended() ||
		!fingers[Finger::TYPE_INDEX].isExtended() ||
		!fingers[Finger::TYPE_MIDDLE].isExtended() ||
		fingers[Finger::TYPE_RING].isExtended() ||
		fingers[Finger::TYPE_PINKY].isExtended())
	{
		return true;
	}

	return false;
}

void VPose::track(const Leap::Frame& frame)
{
	FingerList fingers = hand().fingers();
	Finger index = fingers[Finger::TYPE_INDEX];
	Finger middle = fingers[Finger::TYPE_MIDDLE];
	float dist = (index.tipPosition() - middle.tipPosition()).magnitude();

	bool was_closed = closed_;
	closed_ = (dist <= max_separation_);

	if (open_fn_ && was_closed && !closed_) {
		open_fn_(frame);
	} else if (close_fn_ && !was_closed && closed_) {
		close_fn_(frame);
	}
}