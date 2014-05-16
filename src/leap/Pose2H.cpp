#include "Pose2H.h"

using namespace Leap;

Pose2H::Pose2H() : 
	max_hand_engage_speed_(0.f), 
	exit_speed_(500.0f),
	disengage_on_exit_(false)
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

	if (disengage_on_exit_) {
		if (left_.palmVelocity().z >= exit_speed_ || right_.palmVelocity().z >= exit_speed_) {
			return true;
		}
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

Vector Pose2H::handsCenter(bool stabilized) const
{
	if (stabilized) {
		return (left_.stabilizedPalmPosition() + right_.stabilizedPalmPosition()) * 0.5f;
	}
	return (left_.palmPosition() + right_.palmPosition()) * 0.5f;
}

Vector Pose2H::handsCenterEngaged(bool stabilized) const
{
	if (stabilized) {
		return (left_engaged_.stabilizedPalmPosition() + right_engaged_.stabilizedPalmPosition()) * 0.5f;
	}
	return (left_engaged_.palmPosition() + right_engaged_.palmPosition()) * 0.5f;
}

Vector Pose2H::handsCenterPrevious(bool stabilized) const
{
	if (stabilized) {
		return (left_previous_.stabilizedPalmPosition() + right_previous_.stabilizedPalmPosition()) * 0.5f;
	}
	return (left_previous_.palmPosition() + right_previous_.palmPosition()) * 0.5f;
}

Vector Pose2H::handsCenterDeltaEngaged(bool stabilized) const
{
	return handsCenter(stabilized) - handsCenterEngaged(stabilized);
}

Vector Pose2H::handsCenterDelta(bool stabilized) const
{
	return handsCenter(stabilized) - handsCenterPrevious(stabilized);
}

float Pose2H::handsSeparation(bool stabilized) const
{
	if (stabilized) {
		return (right_.stabilizedPalmPosition() - left_.stabilizedPalmPosition()).magnitude();
	}
	return (right_.palmPosition() - left_.palmPosition()).magnitude();
}

float Pose2H::handsSeparationEngaged(bool stabilized) const
{
	if (stabilized) {
		return (right_engaged_.stabilizedPalmPosition() - left_engaged_.stabilizedPalmPosition()).magnitude();
	}
	return (right_engaged_.palmPosition() - left_engaged_.palmPosition()).magnitude();
}

float Pose2H::handsSeparationPrevious(bool stabilized) const
{
	if (stabilized) {
		return (right_previous_.stabilizedPalmPosition() - left_previous_.stabilizedPalmPosition()).magnitude();
	}
	return (right_previous_.palmPosition() - left_previous_.palmPosition()).magnitude();
}

float Pose2H::handsSeparationDelta(bool stabilized) const
{
	return handsSeparation(stabilized) - handsSeparationPrevious(stabilized);
}

float Pose2H::handsSeparationDeltaEngaged(bool stabilized) const
{
	return handsSeparation(stabilized) - handsSeparationEngaged(stabilized);
}