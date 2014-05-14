#include "LPose.h"

using namespace Leap;

LPose::LPose() :
	state_(State::open),
	open_function_(nullptr),
	close_function_(nullptr)
{
}

bool LPose::shouldEngage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();

	if (frame.hands().count() != 1)
		return false;

	Hand front_hand = frame.hands().frontmost();

	if (!front_hand.isValid())
		return false;

	FingerList fingers = front_hand.fingers();
	if (!fingers[Finger::TYPE_THUMB].isExtended() ||
		!fingers[Finger::TYPE_INDEX].isExtended() ||
		fingers[Finger::TYPE_MIDDLE].isExtended() ||
		fingers[Finger::TYPE_RING].isExtended() ||
		fingers[Finger::TYPE_PINKY].isExtended())
	{
		return false;
	}

	hand_engaged_ = hand_current_ = front_hand;
	return true;
}

bool LPose::shouldDisengage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();

	if (frame.hands().count() != 1)
		return true;

	hand_current_ = frame.hand(hand_current_.id());
	if (!hand_current_.isValid())
		return true;

	FingerList fingers = hand_current_.fingers();

	// it is intentional I'm not checking thumb: it could be
	// perceived as not extended when tucked into hand
	if (!fingers[Finger::TYPE_INDEX].isExtended() ||
		fingers[Finger::TYPE_MIDDLE].isExtended() ||
		fingers[Finger::TYPE_RING].isExtended() ||
		fingers[Finger::TYPE_PINKY].isExtended())
	{
		return true;
	}

	return false;
}

void LPose::track(const Leap::Controller& controller)
{
	Frame frame = controller.frame();
	hand_current_ = frame.hand(hand_current_.id());

	Finger index = hand_current_.fingers()[Finger::TYPE_INDEX];
	Finger thumb = hand_current_.fingers()[Finger::TYPE_THUMB];

	// consider angle in XZ plane only
	Vector u = index.direction();
	u.y = 0;
	u = u.normalized();
	Vector v = thumb.direction();
	v.y = 0;
	v = v.normalized();
	float angle = u.angleTo(v);

	State prev = state_;
	state_ = (angle > 0.3f) ? State::open : State::closed;

	if (prev == State::open && state_ == State::closed && close_function_) {
		close_function_(controller);
	} else if (prev == State::closed && state_ == State::open && open_function_) {
		open_function_(controller);
	}

	PoseTracker::track(controller);
}