#include "PinchPose.h"

using namespace std;
using namespace Leap;
using namespace std::chrono;

PinchPose::PinchPose() : state_(State::open), last_pinch_(high_resolution_clock::now())
{
}

bool PinchPose::shouldEngage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();
	HandList hands = frame.hands();

	if (hands.count() == 0)
		return false;

	if (frame.fingers().extended().count() < 3)
		return false;

	state_ = State::open;
	return true;
}

bool PinchPose::shouldDisengage(const Leap::Controller& controller)
{
	Frame frame = controller.frame();

	if (frame.hands().count() == 0)
		return true;

	if (frame.fingers().extended().count() < 3)
		return true;

	return false;
}

void PinchPose::track(const Leap::Controller& controller)
{
	Frame frame = controller.frame();

	// left and right hands made invalid, then detected
	left_ = Hand{};
	right_ = Hand{};
	for (const Leap::Hand& hand : frame.hands()) {
		if (hand.isLeft())
			left_ = hand;
		else if (hand.isRight())
			right_ = hand;
	}

	State prev_state = state_;
	bool pinch_left = left_.isValid() && left_.pinchStrength() > 0.9f;
	bool pinch_right = right_.isValid() && right_.pinchStrength() > 0.9f;

	if (pinch_left && pinch_right) {
		state_ = State::pinch_both;
	} else if (pinch_left) {
		state_ = State::pinch_left;
		if (prev_state == State::open) {
			left_pinched_ = left_;
			last_pinch_ = high_resolution_clock::now();
		}
	} else if (pinch_right) {
		state_ = State::pinch_right;
		if (prev_state == State::open) {
			right_pinched_ = right_;
			auto current_time = high_resolution_clock::now();
			if (dbl_pinch_fn_){
				milliseconds time_since_pinch = duration_cast<milliseconds>(current_time - last_pinch_);
				if (time_since_pinch.count() < 750) {
					dbl_pinch_fn_(controller);
				}
			}
			last_pinch_ = current_time;
		}
	} else {
		state_ = State::open;
	}

	if (state_fn_ && state_ != prev_state) {
		state_fn_(controller, prev_state, state_);
	}

	PoseTracker::track(controller);
}

Vector PinchPose::positionStabilized() const
{
	if (left_.isValid() && right_.isValid()) {
		return (left_.stabilizedPalmPosition() + right_.stabilizedPalmPosition()) * 0.5f;
	} else if (left_.isValid()) {
		return left_.stabilizedPalmPosition();
	} else if (right_.isValid()) {
		return right_.stabilizedPalmPosition();
	} else {
		return Vector{};
	}
}