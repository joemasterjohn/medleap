#include "LPose.h"

using namespace Leap;

LPose::LPose() :
		m_thumb_spd_thresh(200),
		m_index_spd_thresh(50),
		m_delta_spd_thresh(300)
{
	engageDelay(std::chrono::milliseconds(500));
	disengageDelay(std::chrono::milliseconds(500));
}

bool LPose::shouldEngage(const Leap::Controller& controller)
{
	FingerList fingers = controller.frame().fingers();
	Frame frame = controller.frame();

	// must be a single hand
	if (frame.hands().count() != 1)
		return false;

	// must be at two fingers to be tracking
	if (fingers.count() != 2)
		return false;

	Finger index = fingers.frontmost();
	Finger thumb = fingers.leftmost();
	float indexSpd = index.tipVelocity().magnitude();
	float thumbSpd = thumb.tipVelocity().magnitude();
	float deltaSpd = abs(indexSpd - thumbSpd);


	// index finger is not moving much, but the thumb is
	if (indexSpd < m_index_spd_thresh && thumbSpd > m_thumb_spd_thresh)
		return true;

	// fingers are moving, but the thumb is moving away from index quickly
	if (deltaSpd > m_delta_spd_thresh)
		return true;

	return false;
}

bool LPose::shouldDisengage(const Leap::Controller& controller)
{
	FingerList fingers = controller.frame().fingers();

	// hand has disappeared, so must stop tracking
	if (fingers.count() < 1)
		return true;

	Finger index = fingers.frontmost();
	Finger thumb = fingers.leftmost();
	float indexSpd = index.tipVelocity().magnitude();
	float thumbSpd = thumb.tipVelocity().magnitude();
	float deltaSpd = abs(indexSpd - thumbSpd);

	// index finger is not moving much, but the thumb is
	if (indexSpd < m_index_spd_thresh && thumbSpd > m_thumb_spd_thresh)
		return true;

	// fingers are moving, but the thumb is moving away from index quickly
	if (deltaSpd > m_delta_spd_thresh)
		return true;

	return false;
}

void LPose::engage(const Leap::Controller& controller)
{
	// TODO remove this and put in shouldEngage
	m_engage_tip_pos = controller.frame().fingers().frontmost().tipPosition();
	PoseTracker::engage(controller);
}

void LPose::track(const Leap::Controller& controller)
{
	m_delta_tip_pos = controller.frame().fingers().frontmost().tipPosition() - m_engage_tip_pos;
	PoseTracker::track(controller);
}