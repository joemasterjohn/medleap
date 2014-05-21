#include "camera.h"

using namespace gl;

Camera::Camera() : yaw_(0.0f), pitch_(0.0f), radius_(1)
{
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
}

void Camera::projection(const Mat4& projection)
{
    projection_ = projection;
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