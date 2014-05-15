#include "LeapCameraControl.h"
#include "main/MainController.h"

using namespace Leap;
using namespace std;
using namespace gl;

LeapCameraControl::LeapCameraControl() : tracking_(false)
{
	fist_pose_.engageFunction(bind(&LeapCameraControl::grab, this, placeholders::_1));
	fist_pose_.disengageFunction(bind(&LeapCameraControl::release, this, placeholders::_1));
}

void LeapCameraControl::update(const Leap::Controller& controller)
{
	//Frame frame = controller.frame();
	//Frame prev_frame = controller.frame(1);
	//HandList hands = frame.hands();

	//if (hands.count() != 2) {
	//	tracking_ = false;
	//	return;
	//}

	//Hand left_hand = hands.leftmost();
	//Hand right_hand = hands.rightmost();

	//if (left_hand.fingers().count() < 2) {
	//	tracking_ = false;
	//	return;
	//}

	//if (right_hand.fingers().count() < 2) {
	//	tracking_ = false;
	//	return;
	//}

	//if (!prev_frame.hand(left_hand.id()).isValid()) {
	//	tracking_ = false;
	//	return;
	//}

	//if (!prev_frame.hand(right_hand.id()).isValid()) {
	//	tracking_ = false;
	//	return;
	//}

	//tracking_ = true;

	//// if just one hand moving: rotation
	//// if both h ands moving: translation
	//VolumeController& vc = MainController::getInstance().volumeController();
	//Camera& camera = vc.getCamera();

	//Vector lht = left_hand.translation(prev_frame);
	//Vector rht = right_hand.translation(prev_frame);
	//Vector diff = lht - rht;
	//bool sameDir = lht.dot(rht) > 0;

	//float s = 1.5f;
	//bool rotate = lht.magnitude() > s * rht.magnitude() || rht.magnitude() > s * lht.magnitude();

	//Vector center = (left_hand.stabilizedPalmPosition() + right_hand.stabilizedPalmPosition()) / 2.0f;
	//Vector prev_center = (prev_frame.hand(left_hand.id()).stabilizedPalmPosition() + prev_frame.hand(right_hand.id()).stabilizedPalmPosition()) / 2.0f;
	//Vector center_delta = center - prev_center;

	//camera.yaw += center_delta.x / 25.0f;
	//camera.pitch = clamp(camera.pitch - center_delta.y / 25.0f, -pi_over_2, pi_over_2);

	//vc.getCamera().setView(translation(0.0f, 0.0f, -1.0f) * rotationX(camera.pitch) * rotationY(camera.yaw));

	//vc.markDirty();


	fist_pose_.update(controller.frame());

	if (fist_pose_.tracking()) {

		if (fist_pose_.state() == FistPose::State::open) {

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

		} else if (fist_pose_.state() == FistPose::State::closed) {
			Camera& camera = MainController::getInstance().volumeController().getCamera();

			Vector a = fist_pose_.hand().palmPosition();
			Vector b = fist_pose_.handPrevious().palmPosition();
			Vector t = (a - b) / 300.0f;

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