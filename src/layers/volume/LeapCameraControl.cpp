#include "LeapCameraControl.h"
#include "main/MainController.h"

using namespace Leap;
using namespace std;
using namespace gl;

LeapCameraControl::LeapCameraControl()
{
	grab_tracker_.engageFunction(bind(&LeapCameraControl::grab, this, placeholders::_1));
	grab_tracker_.disengageFunction(bind(&LeapCameraControl::release, this, placeholders::_1));
}

void LeapCameraControl::update(const Leap::Controller& controller)
{
	grab_tracker_.update(controller);

	if (grab_tracker_.tracking()) {
		Camera& camera = MainController::getInstance().volumeController().getCamera();

		Vector t = grab_tracker_.palmPosDelta(controller.frame()) / 100.0f;

		Mat4 eye2world = camera.getView().rotScale().transpose();
		Vec4 v = eye2world * Vec4(t.x, t.y, t.z, 0);

		std::cout << t << std::endl;

		// relative to camera view
		camera.setView(old_view_ * gl::translation(v.x, v.y, v.z));
		MainController::getInstance().volumeController().markDirty();
	}
}

void LeapCameraControl::grab(const Leap::Controller& controller)
{
	Camera& camera = MainController::getInstance().volumeController().getCamera();
	old_view_ = camera.getView();
}

void LeapCameraControl::release(const Leap::Controller& controller)
{
}