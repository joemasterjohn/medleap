#include "LeapCameraControl.h"
#include "main/MainController.h"

using namespace Leap;
using namespace std;
using namespace gl;

LeapCameraControl::LeapCameraControl() : tracking_(false), mouse_drag_l_(false), mouse_drag_r_(false)
{
	poses_.fist().enabled(true);
	poses_.fist().maxHandEngageSpeed(50.0f);
	poses_.l().enabled(true);
    poses_.l().maxHandEngageSpeed(200.0f);
}

Camera& LeapCameraControl::camera()
{
	return MainController::getInstance().volumeController().getCamera();
}

void LeapCameraControl::update(const Leap::Controller& controller, const Leap::Frame& frame)
{
    tracking_ = false;
	poses_.update(frame);
	if (poses_.fist().tracking() && poses_.fist().state() == FistPose::State::closed) {
		tracking_ = true;
        leapRotate();
	} else if (poses_.l().tracking()) {
        tracking_ = true;
        auto& lsc = MainController::getInstance().leapStateController();
        lsc.increaseBrightness(LeapStateController::icon_l_open);
		leapTranslate();
	}
}

void LeapCameraControl::leapTranslate()
{
	if (poses_.l().isClosed()) {
		Vector t = poses_.l().handPositionDelta() / 300.0f;
		Mat4 eye2world = camera().view().rotScale().transpose();
		Vec4 v = Vec4(t.x, t.y, t.z, 0);
		move(eye2world * -v);
	}
}

void LeapCameraControl::leapRotate()
{
	Vector a = poses_.fist().hand().stabilizedPalmPosition();
	Vector b = poses_.fist().handPrevious().stabilizedPalmPosition();
	Vector v = (a - b) / 200.0f;

	float d_yaw = v.x * pi;
	float d_pitch = -v.y;
	rotate(d_yaw, d_pitch);

	float palm_speed = poses_.fist().hand().palmVelocity().magnitude();
	if (palm_speed > 0) {
		float z_speed = abs(poses_.fist().hand().palmVelocity().z);
		float z_scale = (z_speed / 120.0f) / (palm_speed / 100.0f);
		float d_radius = -poses_.fist().handPositionDelta().z / 100.0f * min(1.0f, z_scale * z_scale);
		zoom(d_radius);
	}
}

void LeapCameraControl::mouseButton(int button, int action, int mods, double x, double y)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		mouse_drag_l_ = action == GLFW_PRESS;
		if (mouse_drag_l_) {
			mouse_drag_start_x_ = x;
			mouse_drag_start_y_ = y;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		mouse_drag_r_ = action == GLFW_PRESS;
		if (mouse_drag_r_) {
			mouse_drag_start_x_ = x;
			mouse_drag_start_y_ = y;
		}
	}
}

void LeapCameraControl::mouseMotion(double x, double y)
{
	double dx = x - mouse_last_x_;
	double dy = y - mouse_last_y_;

	if (mouse_drag_l_) {
		float d_yaw = static_cast<float>(dx * 0.005);
		float d_pitch = static_cast<float>(-dy * 0.005);
		rotate(d_yaw, d_pitch);
	}

	if (mouse_drag_r_) {
		Vec4 d;
		d.x = -static_cast<float>(dx * 0.001f);
		d.y = -static_cast<float>(dy * 0.001f);
		move(camera().viewInverse() * d);
	}

	mouse_last_x_ = x;
	mouse_last_y_ = y;
}

void LeapCameraControl::mouseScroll(double x, double y)
{
	if (mouse_drag_r_) {
		move(camera().viewInverse() * Vec4(0.0f, 0.0f, -y * 0.05f, 0.0f));
	} else {
		zoom(y * 0.05);
	}
}

void LeapCameraControl::move(const Vec3& delta)
{

    
    
	VolumeController& vc = MainController::getInstance().volumeController();

	const Box& bounds = MainController::getInstance().volumeData()->getBounds();
	Vec3 new_center = bounds.clamp(camera().center() + delta);
	camera().center(new_center);

	vc.draw_cursor3D = true;
	vc.maskColor.set(1.0f, 0.5f, 0.5f);
	vc.maskCenter = camera().center();
	vc.markDirty();
}

void LeapCameraControl::rotate(float delta_yaw, float delta_pitch)
{
    auto& lsc = MainController::getInstance().leapStateController();
	lsc.increaseBrightness(LeapStateController::icon_fist);
    
	VolumeController& vc = MainController::getInstance().volumeController();
	camera().yaw(camera().yaw() + delta_yaw);
	camera().pitch(camera().pitch() + delta_pitch);
	vc.markDirty();
}

void LeapCameraControl::zoom(float delta_radius)
{
    auto& lsc = MainController::getInstance().leapStateController();
	lsc.increaseBrightness(LeapStateController::icon_fist);
    
	VolumeController& vc = MainController::getInstance().volumeController();
	camera().radius(camera().radius() + delta_radius);
	vc.markDirty();
}