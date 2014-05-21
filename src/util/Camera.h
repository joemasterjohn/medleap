#ifndef CGL_CAMERA_H_
#define CGL_CAMERA_H_

#include "gl/math/Math.h"
#include <vector>

class Camera
{
public:
    /// Creates a camera. Initially, the view and projection are identity matrices.
    Camera();
    
    /// Returns the view matrix.
	const gl::Mat4& view() const { return view_; }

	/// Inverse of view matrix
	const gl::Mat4& viewInverse() const { return view_inverse_; }
    
    /// Returns the projection matrix.
	const gl::Mat4& projection() const { return projection_; }
    
    /// Returns the eye position in world coordinates; origin in eye coordinates.
	const gl::Vec4& eye() const { return eye_; }
    
    /// Returns the up axis in world coordinates; (0, 1, 0) in eye coordinates.
	const gl::Vec4& up() const { return up_; }
    
    /// Returns the right axis in world coordinates; (1, 0, 0) in eye coordinates.
	const gl::Vec4& right() const { return right_; }
    
    /// Returns the forward axis in world coordinates; (0, 0, -1) in eye coordinates.
	const gl::Vec4& forward() const { return forward_; }

	/// Using perspective projection
	bool perspective() const { return perspective_; }

	/// Set projection type (true = perspective, false = orthographic)
	void perspective(bool perspective);

	/// Set aspect ratio
	void aspect(float aspect);

	/// Current aspect ratio
	float aspect() const { return aspect_; }

	/// Yaw angle radians
	float yaw() const { return yaw_; }

	/// Pitch angle radians
	float pitch() const { return pitch_; }

	/// Distance eye is from center of rotation
	float radius() const { return radius_; }

	/// Center of rotation
	const gl::Vec3& center() const { return center_; }

	/// Set yaw angle radians
	void yaw(float yaw);

	/// Set pitch angle radians
	void pitch(float pitch);

	/// Set distance from center of rotation
	void radius(float radius);

	/// Set center of rotation
	void center(const gl::Vec3& center);

private:
	gl::Mat4 view_;
	gl::Mat4 view_inverse_;
	gl::Mat4 projection_;
	gl::Vec4 eye_;
	gl::Vec4 up_;
	gl::Vec4 right_;
	gl::Vec4 forward_;
	float yaw_;
	float pitch_;
	float radius_;
	gl::Vec3 center_;
	float aspect_;
	bool perspective_;
    
    void update();
};

#endif // CGL_CAMERA_H_
