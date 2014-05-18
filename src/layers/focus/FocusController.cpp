#include "FocusController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;
using namespace Leap;

FocusController::FocusController()
{
}

void FocusController::gainFocus()
{
	MainController::getInstance().setMode(MainController::MODE_3D);
	MainController::getInstance().showTransfer1D(false);
	VolumeController& vc = MainController::getInstance().volumeController();
	vc.draw_bounds = true;
	vc.draw_cursor3D = true;
	vc.use_context = true;
	vc.markDirty();
}

void FocusController::loseFocus()
{
	VolumeController& vc = MainController::getInstance().volumeController();
	vc.draw_bounds = false;
	vc.draw_cursor3D = false;
	vc.use_context = false;
	vc.markDirty();
}

bool FocusController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	point_pose_.update(frame);

	if (point_pose_.tracking()) {
		moveCursor();
	} else {
		camera_control_.update(controller, frame);
	}

	return false;
}

void FocusController::moveCursor()
{
	MainController& mc = MainController::getInstance();
	VolumeController& vc = mc.volumeController();

	vc.maskColor = { 1.0f, 1.0f, 0.0f };

	Mat4 eye2world = vc.getCamera().getView().inverse();
	Vec4 hand_delta_ws = eye2world * point_pose_.handPositionDelta().toVector4<Vec4>();

	cursor_pos_ = mc.volumeData()->getBounds().clamp(cursor_pos_ + hand_delta_ws / 400.0f);

	vc.maskCenter = cursor_pos_;
	vc.markDirty();
}