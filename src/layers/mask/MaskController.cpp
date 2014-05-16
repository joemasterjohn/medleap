#include "MaskController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "BoxMask.h"

using namespace gl;
using namespace std;
using namespace Leap;

MaskController::MaskController() : mask_volume_(new BoxMask(Box(0.02f, 0.02f, 0.02f)))
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
			vc.maskColor = { 1.0f, 1.0f, 0.0f };
			Vector v2 = v_pose_.hand().palmPosition();
			v2 -= controller.frame(1).hand(v_pose_.hand().id()).palmPosition();

			v2 *= 0.5;

			Mat4 eye2world = vc.getCamera().getView().inverse();
			Vec4 v = eye2world * Vec4(v2.x, v2.y, v2.z, 0.0f);

			VolumeData* data = MainController::getInstance().volumeData();
			const Box& b = data->getBounds();

			Vec3 center = mask_volume_->center() + v / 100.0f;
			center = b.clamp(center);
			mask_volume_->center(center);
			vc.maskCenter = center;


			if (v_pose_.isClosed()) {
				vc.maskColor = { 1.0f, 0.0f, 0.0f };
				MaskVolume::Edit edit = mask_volume_->apply(b, vc.maskTexture, MaskVolume::Operation::sub);
				if (!edit.empty()) {
					edits_.push(std::move(edit));
				}
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