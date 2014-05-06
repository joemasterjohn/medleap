#include "Transform.h"

using namespace gl;

Transform::Transform() :
	position_(0.0f, 0.0f, 0.0f),
	x_(1.0f, 0.0f, 0.0f),
	y_(0.0f, 1.0f, 0.0f),
	z_(0.0f, 0.0f, 1.0f)
{
}

Transform::~Transform()
{
}

const Mat4& Transform::matrix() const
{
	return matrix_;
}

const Mat4& Transform::inverse() const
{
	return inverse_;
}

const Vec3& Transform::position() const
{
	return position_;
}

const Vec3& Transform::x() const
{
	return x_;
}

const Vec3& Transform::y() const
{
	return y_;
}

const Vec3& Transform::z() const
{
	return z_;
}

void Transform::matrix(const Mat4& matrix)
{
	matrix_ = matrix;
}

void Transform::position(const Vec3& position)
{
	matrix(translation(position) * matrix_.rotScale());
}

void Transform::update()
{
	inverse_ = matrix_.inverse();
	x_ = inverse_.col(0);
	y_ = inverse_.col(1);
	z_ = inverse_.col(2);
	position_ = inverse_.col(3);
}





float YawPitchTransform::yaw() const
{
	return yaw_;
}

float YawPitchTransform::pitch() const
{
	return pitch_;
}

void YawPitchTransform::yaw(float yaw)
{
	yaw_ = yaw;
	matrix(translation(position_) * rotationY(yaw_) * rotationX(pitch_));
}

void YawPitchTransform::pitch(float pitch)
{
	pitch_ = pitch;
	matrix(translation(position_) * rotationY(yaw_) * rotationX(pitch_));
}

void YawPitchTransform::yawPitch(float yaw, float pitch)
{
	yaw_ = yaw;
	pitch_ = pitch;
	matrix(translation(position_) * rotationY(yaw_) * rotationX(pitch_));
}