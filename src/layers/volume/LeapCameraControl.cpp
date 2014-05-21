#include "LeapCameraControl.h"
#include "main/MainController.h"

using namespace Leap;
using namespace std;
using namespace gl;

LeapCameraControl::LeapCameraControl() : tracking_(false)
{
	poses_.fist().enabled(true);
	poses_.fist().maxHandEngageSpeed(50.0f);
	poses_.fist().minValidFrames(0);

	poses_.carry().enabled(true);
}

void LeapCameraControl::update(const Leap::Controller& controller, const Leap::Frame& frame)
{
	poses_.update(frame);

	auto& vc = MainController::getInstance().volumeController();
	Camera& camera = MainController::getInstance().volumeController().getCamera();

	if (poses_.carry().tracking()) {

		Vector t = poses_.carry().handPositionDelta() / 200.0f;
		Mat4 eye2world = camera.view().rotScale().transpose();
		Vec4 v = Vec4(t.x, t.y, t.z, 0);
		const Box& bounds = MainController::getInstance().volumeData()->getBounds();
		camera.center(bounds.clamp(camera.center() - eye2world * v));

		vc.maskColor.set(1.0f, 0.5f, 0.5f);
		vc.maskCenter = camera.center();
		vc.draw_cursor3D = true;
		MainController::getInstance().volumeController().markDirty();

	} else if (poses_.fist().tracking() && poses_.fist().state() == FistPose::State::closed) {


		Vector a = poses_.fist().hand().stabilizedPalmPosition();
		Vector b = poses_.fist().handPrevious().stabilizedPalmPosition();
		Vector v = (a-b) / 200.0f;
			
		camera.yaw(camera.yaw() + v.x * pi);
		camera.pitch(clamp(camera.pitch() - v.y, -0.5f * pi, 0.5f * pi));

		float palm_speed = poses_.fist().hand().palmVelocity().magnitude();
		if (palm_speed > 0) {
			float z_speed = abs(poses_.fist().hand().palmVelocity().z);
			float z_scale = (z_speed / 120.0f) / (palm_speed / 100.0f);
			camera.radius(camera.radius() - poses_.fist().handPositionDelta().z / 100.0f * min(1.0f, z_scale * z_scale));
		}

		vc.maskColor.set(1.0f, 0.5f, 0.5f);
		vc.maskCenter = camera.center();
		vc.draw_cursor3D = true;
		MainController::getInstance().volumeController().markDirty();
	}
}