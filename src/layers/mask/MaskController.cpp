#include "MaskController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "BoxMask.h"

using namespace gl;
using namespace std;
using namespace Leap;

MaskController::MaskController() : mask_volume_(new BoxMask(Box(0.04f, 0.04f, 0.04f)))
{
	poses_.v().enabled(true);
	poses_.palmsFace().enabled(true);
}

bool MaskController::modal() const
{
	return editing_;
}

void MaskController::gainFocus()
{
	MainController::getInstance().setMode(MainController::MODE_3D);
	VolumeController& vc = MainController::getInstance().volumeController();
	vc.draw_bounds = true;
	vc.draw_cursor3D = true;
	vc.markDirty();
    
    auto& lsc = MainController::getInstance().leapStateController();
	lsc.clear();
	lsc.add(LeapStateController::icon_point_circle, "Main Menu");
	lsc.add(LeapStateController::icon_three_circle, "Options");
    lsc.add(LeapStateController::icon_fist, "Rotate/Zoom");
	lsc.add(LeapStateController::icon_l_open, "Center");
    lsc.add(LeapStateController::icon_v_open, "Mask");
    lsc.add(LeapStateController::icon_palms_face, "Scale");
}

void MaskController::loseFocus()
{
//	VolumeController& vc = MainController::getInstance().volumeController();
//	vc.draw_bounds = false;
//	vc.draw_cursor3D = false;
//	vc.markDirty();
}

bool MaskController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	editing_ = false;
	poses_.update(frame);
	if (!cam_control_.tracking()) {
		VolumeController& vc = MainController::getInstance().volumeController();
		vc.maskColor = { 0.0f, 0.0f, 1.0f };

		if (poses_.v().tracking()) {
			editing_ = true;
			moveCursor();
			if (poses_.v().isClosed()) {
				applyEdit();
			}

			vc.maskGeometry = mask_volume_->geometry();
			vc.markDirty();
		}

		if (poses_.palmsFace().tracking()) {
            MainController::getInstance().leapStateController().increaseBrightness(LeapStateController::icon_palms_face);
			float scale = min(1.05f, max(1.0f + poses_.palmsFace().handsSeparationDelta() / 100.0f, 0.95f));
			mask_volume_->scale(scale);
			vc.maskGeometry = mask_volume_->geometry();
			vc.markDirty();
		}
	}

	if (!poses_.v().tracking()) {
		cam_control_.update(controller, frame);
	}

	return false;
}

void MaskController::moveCursor()
{
    MainController::getInstance().leapStateController().increaseBrightness(LeapStateController::icon_v_open);
	VolumeController& vc = MainController::getInstance().volumeController();

	vc.maskColor = { 1.0f, 1.0f, 0.0f };

	const Mat4& eye2world = vc.getCamera().viewInverse();
	Vec4 hand_delta_ws = eye2world * poses_.v().handPositionDelta().toVector4<Vec4>();
	Vec3 center = mask_volume_->center() + hand_delta_ws / 400.0f;
	center = MainController::getInstance().volumeData()->getBounds().clamp(center);
	mask_volume_->center(center);
	vc.maskCenter = center;
}

void MaskController::applyEdit()
{
    MainController::getInstance().leapStateController().increaseBrightness(LeapStateController::icon_v_open);
	VolumeController& vc = MainController::getInstance().volumeController();

	vc.maskColor = { 1.0f, 0.0f, 0.0f };

	const Box& bounds = MainController::getInstance().volumeData()->getBounds();
	MaskVolume::Edit edit = mask_volume_->apply(bounds, vc.maskTexture, MaskVolume::Operation::sub);
	if (!edit.empty()) {
		edits_.push(std::move(edit));
	}
}