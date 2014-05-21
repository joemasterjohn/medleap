#include "camera.h"

using namespace gl;

Camera::Camera() : yaw_(0.0f), pitch_(0.0f), radius_(1), aspect_(1.0f), perspective_(true)
{
	perspective(perspective_);
	update();
}

void Camera::update()
{
	Mat4 rot_x = rotationX(-pitch_);
	Mat4 rot_y = rotationY(-yaw_);
	Mat4 rot = rot_y * rot_x;

	eye_ = (rot * Vec4::zAxis()) * radius_ + Vec4(center_, 0.0f);
	view_ = lookAt(eye_, center_, Vec3::yAxis());
	view_inverse_ = view_.inverse();
	right_ = view_inverse_.col(0);
	up_ = view_inverse_.col(1);
	forward_ = view_inverse_.col(2) * -1.0f;
	eye_ = view_inverse_.col(3);

	if (!perspective_) {
		perspective(perspective_);
	}
}

void Camera::aspect(float aspect)
{
	aspect_ = aspect;
	perspective(perspective_);
}

void Camera::perspective(bool perspective)
{
	perspective_ = perspective;
	if (perspective_) {
		projection_ = gl::perspective(60.0f * deg_to_rad, aspect_, 0.1f, 15.0f);
	} else {
		float width = tan(30.0f * deg_to_rad) * radius_;
		float height = width / aspect_;
		projection_ = gl::ortho(-width, width, -height, height, -15.0f, 15.0f);
	}
}

void Camera::yaw(float yaw)
{
	yaw_ = yaw;
	update();
}

void Camera::pitch(float pitch)
{
	pitch_ = clamp(pitch, -pi_over_2 + 0.0000001f, pi_over_2 - 0.0000001f);
	update();
}

void Camera::radius(float radius)
{
	radius_ = std::max(0.01f, radius);
	update();
}

void Camera::center(const Vec3& center)
{
	center_.set(center.x, center.y, center.z);
	update();
}