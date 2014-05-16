#include "LeapCameraControl.h"
#include "main/MainController.h"

using namespace Leap;
using namespace std;
using namespace gl;

LeapCameraControl::LeapCameraControl() : tracking_(false)
{
	fist_pose_.maxHandEngageSpeed(0);
	fist_pose_.minValidFrames(0);
	fist_pose_.engageFunction(bind(&LeapCameraControl::grab, this, placeholders::_1));
	fist_pose_.disengageFunction(bind(&LeapCameraControl::release, this, placeholders::_1));
}

void LeapCameraControl::update(const Leap::Controller& controller, const Leap::Frame& frame)
{
	fist_pose_.update(frame);

	if (fist_pose_.tracking() && fist_pose_.state() == FistPose::State::closed) {
			Camera& camera = MainController::getInstance().volumeController().getCamera();

			Vector a = fist_pose_.hand().stabilizedPalmPosition();
			Vector b = fist_pose_.handPrevious().stabilizedPalmPosition();
			Vector v = (a-b) / 100.0f;
			
			camera.yaw += v.x * gl::pi;
			camera.pitch = gl::clamp(camera.pitch - v.y, -0.5f * gl::pi, 0.5f * gl::pi);

			Mat4 m_center = gl::translation(camera.center);
			Mat4 m_pitch = gl::rotationX(camera.pitch);
			Mat4 m_yaw = gl::rotationY(camera.yaw);
	
			camera.setView(m_center * m_pitch * m_yaw);
			MainController::getInstance().volumeController().markDirty();
	}

	carry_pose_.update(frame);
	if (carry_pose_.tracking()) {
		Camera& camera = MainController::getInstance().volumeController().getCamera();

		Vector t = carry_pose_.handsCenterDelta() / 300.0f;

		Mat4 eye2world = camera.getView().rotScale().transpose();
		Vec4 v = Vec4(t.x, t.y, t.z, 0);

		camera.center += v;
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

	//std::cout << "ENGAGE" << std::endl;
	//std::cout << "valid       : " << grab_tracker_.handCurrent().isValid() << std::endl;
	//std::cout << "fingers     : " << grab_tracker_.handEngaged().fingers().count() << std::endl;
	//std::cout << "time visible: " << grab_tracker_.handEngaged().timeVisible() << std::endl;
	//std::cout << "sphere rad  : " << grab_tracker_.handEngaged().sphereRadius() << std::endl;
	//std::cout << "speed       : " << grab_tracker_.handEngaged().palmVelocity().magnitude() << std::endl;
	//std::cout << grab_tracker_.palmPosEngaged() << std::endl;
	//std::cout << std::endl;
}

void LeapCameraControl::release(const Leap::Frame& controller)
{

}