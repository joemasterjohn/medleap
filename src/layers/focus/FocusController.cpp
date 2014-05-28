#include "FocusController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;
using namespace Leap;

FocusController::FocusController()
{
	poses_.v().enabled(true);
	poses_.palmsFace().enabled(true);
}

void FocusController::gainFocus()
{
	MainController::getInstance().showTransfer1D(false);
	MainController::getInstance().setMode(MainController::MODE_3D);
	VolumeController& vc = MainController::getInstance().volumeController();
	vc.draw_bounds = true;
	vc.draw_cursor3D = true;
	vc.use_context = true;
	vc.markDirty();
    
    auto& lsc = MainController::getInstance().leapStateController();
	lsc.clear();
	lsc.add(LeapStateController::icon_point_circle, "Main Menu");
	lsc.add(LeapStateController::icon_three_circle, "Options");
    lsc.add(LeapStateController::icon_fist, "Rotate/Zoom");
	lsc.add(LeapStateController::icon_l_open, "Center");
    lsc.add(LeapStateController::icon_v_open, "Focus");
    lsc.add(LeapStateController::icon_palms_face, "Scale");
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
	poses_.update(frame);

	if (poses_.v().tracking()) {
		moveCursor();
	}

	if (poses_.palmsFace().tracking()) {
		scaleCursor();
	} else {
		camera_control_.update(controller, frame);
	}
	

	return false;
}

void FocusController::moveCursor()
{
    
    auto& lsc = MainController::getInstance().leapStateController();
    lsc.increaseBrightness(LeapStateController::icon_v_open);
    
	MainController& mc = MainController::getInstance();
	VolumeController& vc = mc.volumeController();

	vc.maskColor = { 1.0f, 1.0f, 0.0f };

	const Mat4& eye2world = vc.getCamera().viewInverse();
	Vec4 hand_delta_ws = eye2world * poses_.v().handPositionDelta().toVector4<Vec4>();

	cursor_pos_ = mc.volumeData()->getBounds().clamp(cursor_pos_ + hand_delta_ws / 400.0f);

	vc.maskCenter = cursor_pos_;
	vc.markDirty();
}

void FocusController::scaleCursor()
{
    MainController::getInstance().leapStateController().increaseBrightness(LeapStateController::icon_palms_face);
    
	MainController& mc = MainController::getInstance();
	VolumeController& vc = mc.volumeController();
	
	PalmsFacePose& pose = poses_.palmsFace();
	float delta = pose.handsSeparationDelta() / 1000.0f;

	vc.cursorRadius += delta;


	vc.markDirty();
}