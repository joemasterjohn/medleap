#include "MaskController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;
using namespace Leap;

MaskController::MaskController() : mask_volume_(new BoxMask(Box(0.1f, 0.1f, 0.1f)))
{
	VolumeController& vc = MainController::getInstance().volumeController();
}

bool MaskController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	tracker_.update(controller);
	if (tracker_.tracking()) {
		VolumeController& vc = MainController::getInstance().volumeController();
		Vector v2 = tracker_.palmPosCurrent();
		v2 -= controller.frame(1).hand(tracker_.handCurrent().id()).palmPosition();

		Mat4 eye2world = vc.getCamera().getView().inverse();
		Vec4 v = eye2world * Vec4(v2.x, v2.y, v2.z, 0.0f);

		VolumeData* data = MainController::getInstance().volumeData();
		const Box& b = data->getBounds();

		Vec3 center = mask_volume_->center() + v / 100.0f;
		center = b.clamp(center);
		mask_volume_->center(center);



		MaskVolume::Edit edit = mask_volume_->apply(vc.maskTexture, MaskVolume::Operation::sub);
		if (!edit.empty()) {
			edits_.push(std::move(edit));
		}

		vc.maskGeometry = mask_volume_->geometry();

		vc.markDirty();
	}

	return false;
}