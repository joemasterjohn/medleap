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
	return tracker_.state() == CutTracker::State::fingers_acquired || 
		   tracker_.state() == CutTracker::State::tracking;
}

bool MaskController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	tracker_.update(controller);

	VolumeController& vc = MainController::getInstance().volumeController();
	vc.maskColor = { 0.0f, 0.0f, 1.0f };
	if (modal()) {
		vc.maskColor = { 1.0f, 1.0f, 0.0f };
		Vector v2 = tracker_.handCurrent().palmPosition();
		v2 -= controller.frame(1).hand(tracker_.handCurrent().id()).palmPosition();

		v2 *= 0.5;

			Mat4 eye2world = vc.getCamera().getView().inverse();
			Vec4 v = eye2world * Vec4(v2.x, v2.y, v2.z, 0.0f);

			VolumeData* data = MainController::getInstance().volumeData();
			const Box& b = data->getBounds();

			Vec3 center = mask_volume_->center() + v / 100.0f;
			center = b.clamp(center);
			mask_volume_->center(center);
			vc.maskCenter = center;


			if (tracker_.tracking()) {
				vc.maskColor = { 1.0f, 0.0f, 0.0f };
				MaskVolume::Edit edit = mask_volume_->apply(b, vc.maskTexture, MaskVolume::Operation::sub);
				if (!edit.empty()) {
					edits_.push(std::move(edit));
				}
			}

			vc.maskGeometry = mask_volume_->geometry();

			vc.markDirty();
		
	}

	return false;
}