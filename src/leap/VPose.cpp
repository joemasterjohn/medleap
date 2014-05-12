#include "VPose.h"
#include <algorithm>

using namespace Leap;
using namespace std;

VPose::VPose() : 
	state_(State::open),
	max_engage_spd_(75.0f),
	max_separation_(25.0f)
{
	engageDelay(std::chrono::milliseconds(150));
}

bool VPose::shouldEngage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();

	if (frame.hands().count() != 1)
		return false;

	Hand front_hand = frame.hands().frontmost();

	if (!front_hand.isValid())
		return false;

	if (front_hand.palmVelocity().magnitude() > max_engage_spd_)
		return false;

	if (front_hand.confidence() < 0.7f)
		return false;

	FingerList fingers = front_hand.fingers();
	if (fingers[Finger::TYPE_THUMB].isExtended() ||
		!fingers[Finger::TYPE_INDEX].isExtended() ||
		!fingers[Finger::TYPE_MIDDLE].isExtended() ||
		fingers[Finger::TYPE_RING].isExtended() ||
		fingers[Finger::TYPE_PINKY].isExtended())
	{
		return false;
	}

	hand_engaged_ = front_hand;
	return true;
}

bool VPose::shouldDisengage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();

	if (frame.hands().count() == 0)
		return true;

	hand_current_ = frame.hand(hand_engaged_.id());

	if (!hand_current_.isValid()) {
		return true;
	}

	FingerList fingers = hand_current_.fingers();

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

void VPose::track(const Leap::Controller& controller)
{
	hand_current_ = controller.frame().hand(hand_engaged_.id());

	FingerList fingers = hand_current_.fingers();
	Finger index = fingers[Finger::TYPE_INDEX];
	Finger middle = fingers[Finger::TYPE_MIDDLE];
	float dist = (index.tipPosition() - middle.tipPosition()).magnitude();
	state_ = (dist <= max_separation_) ? State::closed : State::open;

	PoseTracker::track(controller);
}