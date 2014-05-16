#include "Pose1H.h"

using namespace Leap;

Pose1H::Pose1H(Pose1H::TrackedHand tracked) : 
	tracked_(tracked), 
	max_hand_engage_speed_(0.f)
{
}

Pose1H::~Pose1H()
{
}

bool Pose1H::shouldEngage(const Frame& frame)
{
	const HandList& hands = frame.hands();

	if (hands.count() == 0) {
		return false;
	}

	hand_ = Hand{};
	switch (tracked_)
	{
	case TrackedHand::left:
		for (const Hand& hand : hands) {
			if (hand.isLeft()) {
				hand_ = hand;
				break;
			}
		}
		break;
	case TrackedHand::right:
		for (const Hand& hand : hands) {
			if (hand.isRight()) {
				hand_ = hand;
				break;
			}
		}
		break;
	case TrackedHand::front:
		hand_ = hands.frontmost();
		break;
	case TrackedHand::first:
		if (hands.count() == 1) {
			hand_ = hands[0];
		}
		break;
	case TrackedHand::second:
		if (hands.count() == 2) {
			hand_ = (hands[0].timeVisible() < hands[1].timeVisible()) ? hands[0] : hands[1];
		}
		break;
	}

	if (!hand_.isValid()) {
		return false;
	}

	if (max_hand_engage_speed_ > 0) {
		if (hand_.palmVelocity().magnitude() > max_hand_engage_speed_) {
			return false;
		}
	}

	return true;
}

bool Pose1H::shouldDisengage(const Frame& frame)
{
	hand_previous_ = hand_;
	hand_ = frame.hand(hand_.id());

	if (!hand_.isValid()) {
		return true;
	}

	return false;
}

void Pose1H::engage(const Frame& frame)
{
	hand_engaged_ = hand_;
	hand_previous_ = hand_;
}

Vector Pose1H::handPosition(bool stabilized) const
{
	return stabilized ? hand_.stabilizedPalmPosition() : hand_.palmPosition();
}

Vector Pose1H::handPositionEngaged(bool stabilized) const
{
	return stabilized ? hand_engaged_.stabilizedPalmPosition() : hand_engaged_.palmPosition();
}

Vector Pose1H::handPositionPrevious(bool stabilized) const
{
	return stabilized ? hand_previous_.stabilizedPalmPosition() : hand_previous_.palmPosition();
}

Vector Pose1H::handPositionDeltaEngaged(bool stabilized) const
{
	return handPosition(stabilized) - handPositionEngaged(stabilized);
}

Vector Pose1H::handPositionDelta(bool stabilized) const
{
	return handPosition(stabilized) - handPositionPrevious(stabilized);
}