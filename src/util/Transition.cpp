#include "Transition.h"
#include "gl/math/Math.h"

using namespace std::chrono;

Transition::Transition(milliseconds time_to_complete) :
	time_to_complete_(time_to_complete),
	progress_(0.0f),
	state_(State::empty)
{
}

float Transition::progress() const
{
	return progress_;
}

bool Transition::empty() const
{
	return state_ == State::empty;
}

bool Transition::full() const
{
	return state_ == State::full;
}

bool Transition::increasing() const
{
	return state_ == State::increase;
}

bool Transition::decreasing() const
{
	return state_ == State::decrease;
}

bool Transition::idle() const
{
	return state_ == State::empty || state_ == State::full;
}

Transition::State Transition::state() const
{
	return state_;
}

void Transition::state(Transition::State state)
{
	switch (state)
	{
	case State::empty:
		progress_ = 0.0f;
		break;
	case State::full:
		progress_ = 1.0f;
		break;
	case State::increase:
		if (full()) {
			state = State::full;
		}
		break;
	case State::decrease:
		if (empty()) {
			state = State::empty;
		}
		break;
	}

	state_ = state;
}

void Transition::update(milliseconds elapsed)
{
	switch (state_)
	{
	case State::empty:
	case State::full:
		return;
	case State::increase:
		progress_ += static_cast<float>(elapsed.count()) / time_to_complete_.count();
		if (progress_ >= 1.0f) {
			progress_ = 1.0f;
			state_ = State::full;
		}
		break;
	case State::decrease:
		progress_ -= static_cast<float>(elapsed.count()) / time_to_complete_.count();
		if (progress_ <= 0.0f) {
			progress_ = 0.0f;
			state_ = State::empty;
		}
		break;
	}
}