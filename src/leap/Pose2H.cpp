#include "Pose2H.h"

using namespace Leap;

Pose2H::Pose2H() : max_hand_engage_speed_(0.f)
{
}

Pose2H::~Pose2H()
{
}

bool Pose2H::shouldEngage(const Frame& frame)
{
	const HandList& hands = frame.hands();

	if (hands.count() != 2) {
		return false;
	}

	if (!hands[0].isValid() || !hands[1].isValid()) {
		return false;
	}

	if (hands[0].isLeft()) {
		left_ = hands[0];
		right_ = hands[1];
	} else {
		left_ = hands[1];
		right_ = hands[0];
	}

	if (max_hand_engage_speed_ > 0) {
		float l_speed = left_.palmVelocity().magnitude();
		float r_speed = right_.palmVelocity().magnitude();
		if (l_speed > max_hand_engage_speed_ || r_speed > max_hand_engage_speed_) {
			return false;
		}
	}

	return true;
}

bool Pose2H::shouldDisengage(const Frame& frame)
{
	left_previous_ = left_;
	left_ = frame.hand(left_.id());
	if (!left_.isValid()) {
		return true;
	}

	right_previous_ = right_;
	right_ = frame.hand(right_.id());
	if (!right_.isValid()) {
		return true;
	}

	return false;
}

void Pose2H::engage(const Frame& frame)
{
	left_engaged_ = left_;
	left_previous_ = left_;

	right_engaged_ = right_;
	right_previous_ = right_;
}

void Pose2H::disengage(const Frame& frame)
{
}

void Pose2H::track(const Frame& frame)
{
}