#include "CutTracker.h"
#include <algorithm>

using namespace Leap;
using namespace std;

CutTracker::CutTracker() : 
	state_(State::searching),
	max_engage_spd_(75.0f),
	max_separation_(60.0f)
{
	engageDelay(std::chrono::milliseconds(150));
}

vector<Finger> CutTracker::sortFingers(const Hand& hand)
{
	static auto comp = [](const Finger& a, const Finger& b)->bool {
		return a.tipPosition().z < b.tipPosition().z;
	};

	vector<Finger> result;

	for (const Finger& f : hand.fingers()) {
		result.push_back(f);
	}

	sort(result.begin(), result.end(), comp);

	return result;
}

void CutTracker::checkHandStop(const Leap::Controller& controller)
{
	Frame frame = controller.frame();
	Hand front_hand = frame.hands().frontmost();

	if (frame.hands().count() != 1)
		return;

	if (!front_hand.isValid())
		return;

	if (front_hand.palmVelocity().magnitude() > max_engage_spd_)
		return;

	hand_engaged_ = front_hand;
	hand_current_ = front_hand;
	state_ = State::hand_stopped;
}

void CutTracker::acquireFingers(const Leap::Controller& controller)
{
	Frame frame = controller.frame();
	hand_current_ = frame.hand(hand_current_.id());

	if (frame.hands().count() != 1) {
		cout << "two hands" << endl;
		state_ = State::searching;
		return;
	}

	if (!hand_current_.isValid()) {
		cout << "INVALID HAND" << endl;
		state_ = State::searching;
		return;
	}

	if (hand_current_.fingers().count() < 2) {
		cout << "TOO few fingers" << endl;
		state_ = State::searching;
		return;
	}

	if (hand_current_.fingers().count() > 2) {
		cout << "TOO many fingers" << endl;
		state_ = State::searching;
		return;
	}

	vector<Finger> sorted_fingers = sortFingers(hand_current_);

	closest_finger_ = sorted_fingers[0];
	second_closest_finger_ = sorted_fingers[1];

	if (!closest_finger_.isValid()) {
		cout << "closest invalid" << endl;
		state_ = State::searching;
		return;
	}

	if (!second_closest_finger_.isValid()) {
		cout << "2nd closest invalid" << endl;

		state_ = State::searching;
		return;
	}

	if (abs(closest_finger_.direction().z) < 0.9f) {
		cout << "closest dir bad" << endl;
		state_ = State::searching;
		return;
	}

	if (abs(second_closest_finger_.direction().z) < 0.9f) {
		cout << "2md closest dir bad" << endl;
		state_ = State::searching;
		return;
	}

	Vector v = closest_finger_.tipPosition() - second_closest_finger_.tipPosition();
	float dist = v.magnitude();

	if (dist < max_separation_) {
		state_ = State::fingers_acquired;
	}
}


void CutTracker::fingerClose(const Leap::Controller& controller)
{
	const Frame& frame = controller.frame();
	Finger a = frame.finger(closest_finger_.id());
	Finger b = frame.finger(second_closest_finger_.id());

	hand_current_ = controller.frame().hand(hand_engaged_.id());


	if (!hand_current_.isValid()) {
		state_ = State::searching;
		return;
	}

	if (hand_current_.fingers().count() > 2) {
		state_ = State::searching;
		return;
	}

	if ((a.isValid() && !b.isValid()) || (!a.isValid() && b.isValid())) {
		Vector d = a.isValid() ? a.direction() : b.direction();
		if (abs(d.z) > 0.8) {
			state_ = State::tracking;
			return;
		}
	}

	float dist = (a.tipPosition() - b.tipPosition()).magnitude();
	if (dist < 10.0f) {
		hand_engaged_ = hand_current_;
		state_ = State::tracking;
		return;
	}
}

bool CutTracker::shouldEngage(const Leap::Controller& controller)
{
	switch (state_)
	{
	case State::searching:
		checkHandStop(controller);
		break;
	case State::hand_stopped:
		acquireFingers(controller);
		break;
	case State::fingers_acquired:
		fingerClose(controller);
		break;
	case State::tracking:
		return true;
	}

	return false;
}

bool CutTracker::shouldDisengage(const Leap::Controller& controller)
{
	hand_current_ = controller.frame().hand(hand_engaged_.id());

	if (!hand_current_.isValid()) {
		state_ = State::searching;
		return true;
	}

	int numFingers = hand_current_.fingers().count();

	if (numFingers > 3) {
		state_ = State::searching;
		return true;
	}

	// check if one of the other fingers is close the frontmost
	if (numFingers > 1) {
		Finger frontmost = hand_current_.fingers().frontmost();
		for (const Finger& other : hand_current_.fingers()) {
			if (other.id() != frontmost.id()) {
				Vector v = other.tipPosition() - frontmost.tipPosition();
				if (v.magnitude() < 80.0f) {
					state_ = State::searching;
					return true;
				}
			}
		}
	}

	return false;
}

void CutTracker::track(const Leap::Controller& controller)
{
	hand_current_ = controller.frame().hand(hand_engaged_.id());
	LeapTracker::track(controller);
}

void CutTracker::maxEngageSpeed(float speed)
{
	max_engage_spd_ = speed;
}