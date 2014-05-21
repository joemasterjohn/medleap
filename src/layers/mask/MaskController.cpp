#include "MaskController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "BoxMask.h"

using namespace gl;
using namespace std;
using namespace Leap;

MaskController::MaskController() : mask_volume_(new BoxMask(Box(0.04f, 0.04f, 0.04f)))
{
	VolumeController& vc = MainController::getInstance().volumeController();
}

bool MaskController::modal() const
{
	return v_pose_.tracking() || cam_control_.tracking();
}

void MaskController::gainFocus()
{
	MainController::getInstance().setMode(MainController::MODE_3D);
	VolumeController& vc = MainController::getInstance().volumeController();
	vc.draw_bounds = true;
	vc.draw_cursor3D = true;
	vc.markDirty();
}

void MaskController::loseFocus()
{
	VolumeController& vc = MainController::getInstance().volumeController();
	vc.draw_bounds = false;
	vc.draw_cursor3D = false;
	vc.markDirty();
}

bool MaskController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	if (!cam_control_.tracking()) {
		v_pose_.update(frame);

		VolumeController& vc = MainController::getInstance().volumeController();
		vc.maskColor = { 0.0f, 0.0f, 1.0f };

		if (v_pose_.tracking()) {
			moveCursor();
			if (v_pose_.isClosed()) {
				applyEdit();
			}

			vc.maskGeometry = mask_volume_->geometry();
			vc.markDirty();
		}
	}

	if (!v_pose_.tracking()) {
		cam_control_.update(controller, frame);
	}

	return false;
}

void MaskController::moveCursor()
{
	VolumeController& vc = MainController::getInstance().volumeController();

	vc.maskColor = { 1.0f, 1.0f, 0.0f };

	const Mat4& eye2world = vc.getCamera().viewInverse();
	Vec4 hand_delta_ws = eye2world * v_pose_.handPositionDelta().toVector4<Vec4>();
	Vec3 center = mask_volume_->center() + hand_delta_ws / 400.0f;
	center = MainController::getInstance().volumeData()->getBounds().clamp(center);
	mask_volume_->center(center);
	vc.maskCenter = center;
}

void MaskController::applyEdit()
{
	VolumeController& vc = MainController::getInstance().volumeController();

	vc.maskColor = { 1.0f, 0.0f, 0.0f };

	const Box& bounds = MainController::getInstance().volumeData()->getBounds();
	MaskVolume::Edit edit = mask_volume_->apply(bounds, vc.maskTexture, MaskVolume::Operation::sub);
	if (!edit.empty()) {
		edits_.push(std::move(edit));
	}
}