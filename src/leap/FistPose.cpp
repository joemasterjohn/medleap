#include "FistPose.h"

using namespace std;
using namespace Leap;

FistPose::FistPose() : state_(State::open), max_engage_spd_(75.0f)
{
	engageDelay(std::chrono::milliseconds(150));
}

bool FistPose::shouldEngage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();
	Hand potential_hand = frame.hands().frontmost();

	if (frame.hands().count() != 1)
		return false;

	if (!potential_hand.isValid())
		return false;

	if (potential_hand.fingers().extended().count() != 5)
		return false;

	if (potential_hand.palmVelocity().magnitude() > max_engage_spd_)
		return false;

	frames_tracked_ = 0;
	hand_engaged_ = hand_current_ = potential_hand;

	return true;
}

bool FistPose::shouldDisengage(const Leap::Controller& controller)
{
	hand_current_ = controller.frame().hand(hand_engaged_.id());

	if (!hand_current_.isValid())
		return true;

	return false;
}

void FistPose::track(const Leap::Controller& controller)
{
	if (frames_tracked_ > 0) {
		hand_previous_ = controller.frame(1).hand(hand_current_.id());
	}
	if (frames_tracked_ < std::numeric_limits<unsigned>::max()) {
		frames_tracked_++;
	}

	hand_current_ = controller.frame().hand(hand_engaged_.id());

	if (hand_current_.grabStrength() < 0.85f) {
		if (hand_current_.fingers().extended().count() == 5) {
			state_ = State::open;
		} else {
			state_ = State::partial;
		}
	} else {
		state_ = State::closed;
	}

	PoseTracker::track(controller);
}

void FistPose::maxEngageSpeed(float speed)
{
	max_engage_spd_ = speed;
}