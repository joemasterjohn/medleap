#include "LeapCameraControl.h"
#include "main/MainController.h"

using namespace Leap;
using namespace std;
using namespace gl;

LeapCameraControl::LeapCameraControl() : tracking_(false)
{
	poses_.fist().enabled(true);
	poses_.fist().maxHandEngageSpeed(175.0f);
	poses_.fist().minValidFrames(0);
	poses_.fist().engageFunction(bind(&LeapCameraControl::grab, this, placeholders::_1));

	poses_.carry().enabled(true);
}

void LeapCameraControl::update(const Leap::Controller& controller, const Leap::Frame& frame)
{
	poses_.update(frame);

	if (poses_.carry().tracking()) {
		Camera& camera = MainController::getInstance().volumeController().getCamera();

		Vector t = poses_.carry().handPositionDelta() / 300.0f;

		Mat4 eye2world = camera.getView().rotScale().transpose();
		Vec4 v = Vec4(t.x, t.y, t.z, 0);

		camera.center += v;
		Mat4 m_center = gl::translation(camera.center);
		Mat4 m_pitch = gl::rotationX(camera.pitch);
		Mat4 m_yaw = gl::rotationY(camera.yaw);

		camera.setView(m_center * m_pitch * m_yaw);
		MainController::getInstance().volumeController().markDirty();

	} else if (poses_.fist().tracking() && poses_.fist().state() == FistPose::State::closed) {
			Camera& camera = MainController::getInstance().volumeController().getCamera();

			Vector a = poses_.fist().hand().stabilizedPalmPosition();
			Vector b = poses_.fist().handPrevious().stabilizedPalmPosition();
			Vector v = (a-b) / 100.0f;
			
			camera.yaw += v.x * gl::pi;
			camera.pitch = gl::clamp(camera.pitch - v.y, -0.5f * gl::pi, 0.5f * gl::pi);

			Mat4 m_center = gl::translation(camera.center);
			Mat4 m_pitch = gl::rotationX(camera.pitch);
			Mat4 m_yaw = gl::rotationY(camera.yaw);
	
			camera.setView(m_center * m_pitch * m_yaw);
			MainController::getInstance().volumeController().markDirty();
	}
}

void LeapCameraControl::grab(const Leap::Frame& controller)
{
	Camera& camera = MainController::getInstance().volumeController().getCamera();
	old_view_ = camera.getView();
	old_camera_ = camera;
}