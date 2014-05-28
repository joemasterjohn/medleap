#include "PalmsFacePose.h"

using namespace Leap;

PalmsFacePose::PalmsFacePose()
{
	maxHandEngageSpeed(50.0f);
}

bool PalmsFacePose::shouldEngage(const Frame& frame)
{
	if (!Pose2H::shouldEngage(frame)) {
		return false;
	}

	if (std::abs(left().palmNormal().x) < 0.75f) {
		return false;
	}

	if (std::abs(right().palmNormal().x) < 0.75f) {
		return false;
	}

	if (left().palmNormal().dot(right().palmNormal()) > -0.25f) {
		return false;
	}

	return true;
}

bool PalmsFacePose::shouldDisengage(const Frame& frame)
{
	if (Pose2H::shouldDisengage(frame)) {
		return true;
	}


	if (std::abs(left().palmNormal().x) < 0.75f) {
		return true;
	}

	if (std::abs(right().palmNormal().x) < 0.75f) {
		return true;
	}

	if (left().palmNormal().dot(right().palmNormal()) > -0.25f) {
		return true;
	}

	return false;
}

void PalmsFacePose::track(const Frame& frame)
{
}