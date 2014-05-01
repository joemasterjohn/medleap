#include "LeapCameraControl.h"
#include "main/MainController.h"

using namespace Leap;
using namespace std;
using namespace gl;

LeapCameraControl::LeapCameraControl()
{
	grab_tracker_.engageFunction(bind(&LeapCameraControl::grab, this, placeholders::_1));
	grab_tracker_.disengageFunction(bind(&LeapCameraControl::release, this, placeholders::_1));
	finger_tracker_.engageFunction(bind(&LeapCameraControl::grab, this, placeholders::_1));
}

void LeapCameraControl::update(const Leap::Controller& controller)
{
	grab_tracker_.update(controller);

	if (grab_tracker_.tracking() && !finger_tracker_.tracking()) {
		Camera& camera = MainController::getInstance().volumeController().getCamera();

		Vector t = grab_tracker_.palmPosDelta() / 100.0f;

		Mat4 eye2world = camera.getView().rotScale().transpose();
		Vec4 v = eye2world * Vec4(t.x, t.y, t.z, 0);
		
		std::cout << "sphere rad  : " << grab_tracker_.handCurrent().sphereRadius() << std::endl;


		// relative to camera view
		camera.setView(old_view_ * gl::translation(v.x, v.y, v.z));
		MainController::getInstance().volumeController().markDirty();
	} else {
		finger_tracker_.update(controller);
		if (finger_tracker_.tracking()) {
			Vector v = finger_tracker_.centerPosDelta(controller.frame()) / 100.0f;
			Camera& camera = MainController::getInstance().volumeController().getCamera();

			float yaw = v.x * gl::two_pi;
			float pitch = gl::clamp(v.y, -0.5f, 0.5f) * gl::pi;

			Mat4 m_yaw = gl::rotation(yaw, old_camera_.getUp());
			Mat4 m_pitch = gl::rotation(pitch, old_camera_.getRight());
			camera.setView(old_view_ * m_pitch * m_yaw);
			
			MainController::getInstance().volumeController().markDirty();
		}
	}
}

void LeapCameraControl::grab(const Leap::Controller& controller)
{
	Camera& camera = MainController::getInstance().volumeController().getCamera();
	old_view_ = camera.getView();
	old_camera_ = camera;

	std::cout << "ENGAGE" << std::endl;
	std::cout << "valid       : " << grab_tracker_.handCurrent().isValid() << std::endl;
	std::cout << "fingers     : " << grab_tracker_.handEngaged().fingers().count() << std::endl;
	std::cout << "time visible: " << grab_tracker_.handEngaged().timeVisible() << std::endl;
	std::cout << "sphere rad  : " << grab_tracker_.handEngaged().sphereRadius() << std::endl;
	std::cout << "speed       : " << grab_tracker_.handEngaged().palmVelocity().magnitude() << std::endl;
	std::cout << grab_tracker_.palmPosEngaged() << std::endl;
	std::cout << std::endl;
}

void LeapCameraControl::release(const Leap::Controller& controller)
{

}