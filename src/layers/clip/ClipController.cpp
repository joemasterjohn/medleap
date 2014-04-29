#include "ClipController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;
using namespace Leap;

ClipController::ClipController()
{
	hand_tracker_.trackFunction(bind(&ClipController::updateVector, this, placeholders::_1));
}

void ClipController::loseFocus()
{
	hand_tracker_.tracking(false);
}

bool ClipController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	hand_tracker_.update(controller);

	return false;
}

void ClipController::updateVector(const Leap::Controller& controller)
{
	
	Vector t = hand_tracker_.deltaTipPos().normalized();

	VolumeController& vc = MainController::getInstance().volumeController();
	vc.clip_dir_ = { t.x, t.y, t.z };
	vc.markDirty();
}