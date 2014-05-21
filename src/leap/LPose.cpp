#include "LPose.h"

using namespace Leap;
using namespace std::chrono;

LPose::LPose() :
	closed_(false),
	last_close_(high_resolution_clock::now()),
	open_fn_(nullptr),
	close_fn_(nullptr)
{
	maxHandEngageSpeed(55.0f);
}

bool LPose::shouldEngage(const Frame& frame)
{
	if (!Pose1H::shouldEngage(frame)) {
		return false;
	}

	FingerList fingers = hand().fingers();

	thumb_ = fingers[Finger::TYPE_THUMB];
	if (!thumb_.isExtended()) {
		return false;
	}

	// prefer index finger as pointer over middle finger
	pointer_ = Finger{};
	if (fingers[Finger::TYPE_INDEX].isExtended()) {
		pointer_ = fingers[Finger::TYPE_INDEX];
	} else if (fingers[Finger::TYPE_MIDDLE].isExtended()) {
		pointer_ = fingers[Finger::TYPE_MIDDLE];
	}

	if (!pointer_.isValid()) {
		return false;
	}

	if (fingers[Finger::TYPE_RING].isExtended() || fingers[Finger::TYPE_PINKY].isExtended()) {
		return false;
	}

	return true;
}

bool LPose::shouldDisengage(const Frame& frame)
{
	if (Pose1H::shouldDisengage(frame)) {
		return true;
	}

	if (!pointer_.isValid() || !pointer_.isExtended()) {
		return true;
	}

	FingerList fingers = hand().fingers();

	if (fingers[Finger::TYPE_RING].isExtended()) {
		return true;
	}

	if (fingers[Finger::TYPE_PINKY].isExtended()) {
		return true;
	}

	return false;
}

void LPose::track(const Frame& frame)
{
	pointer_ = frame.finger(pointer_.id());
	thumb_ = frame.finger(thumb_.id());

	// consider angle in XZ plane only
	Vector u = pointer_.direction();
	u.y = 0;
	u = u.normalized();
	Vector v = thumb_.direction();
	v.y = 0;
	v = v.normalized();
	float angle = u.angleTo(v);

	bool was_closed = closed_;

	closed_ = (angle <= 0.3f) && fingerMotion() < 300.0f;

	if (hand().confidence() > 0.75f && was_closed && !closed_) {
		if (open_fn_) {
			open_fn_(frame);
		}
		if (click_fn_) {
			auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - last_close_);
			if (elapsed.count() < 200.0f) {
				click_fn_(frame);
			}
		}
	} else if (hand().confidence() > 0.75f && !was_closed && closed_) {
		pointer_closed_ = pointer_;
		if (close_fn_) {
			close_fn_(frame);
		}
		last_close_ = high_resolution_clock::now();
	}
}