#ifndef CGL_CAMERA_H_
#define CGL_CAMERA_H_

#include "gl/math/Math.h"
#include <vector>

class Camera
{
public:
	class Listener
	{
	public:
		virtual ~Listener();
		virtual void cameraUpdated(const Camera& camera) = 0;
	};

    /// Creates a camera. Initially, the view and projection are identity matrices.
    Camera();
    
    /// Returns the view matrix.
	const gl::Mat4& getView() const;
    
    /// Returns the projection matrix.
	const gl::Mat4& getProjection() const;
    
    /// Returns the eye position in world coordinates; origin in eye coordinates.
	const gl::Vec4& getEye() const;
    
    /// Returns the up axis in world coordinates; (0, 1, 0) in eye coordinates.
	const gl::Vec4& getUp() const;
    
    /// Returns the right axis in world coordinates; (1, 0, 0) in eye coordinates.
	const gl::Vec4& getRight() const;
    
    /// Returns the forward axis in world coordinates; (0, 0, -1) in eye coordinates.
	const gl::Vec4& getForward() const;
    
    /// Assigns a new view matrix.
	void setView(const gl::Mat4& view);
    
    /// Assigns a projection matrix.
	void setProjection(const gl::Mat4& projection);
    
    /// Adds a rotation of radians around axis.
	void rotate(float radians, gl::Vec3 axis);
    
    /// Adds a rotation of radians around the X axis.
    void rotateX(float radians);
    
    /// Adds a rotation of radians around the Y axis.
    void rotateY(float radians);
    
    /// Adds a rotation of radians around the Z axis.
    void rotateZ(float radians);
    
    /// Adds a translation of (x, y, z) in world coordinates.
    void translate(float x, float y, float z);
    
    /// Adds a translation of t in world coordinates.
	void translate(const gl::Vec3& t);
    
    /// Adds a translation along the right axis.
    void translateRight(float units);
    
    /// Adds a translation along the left axis.
    void translateLeft(float units);
    
    /// Adds a translation along the up axis.
    void translateUp(float units);
    
    /// Adds a translation along the down axis.
    void translateDown(float units);
    
    /// Adds a translation along the forward axis.
    void translateForward(float units);
    
    /// Adds a translation along the backward axis.
    void translateBackward(float units);
    
	void addListener(Listener* l);

	void removeListener(Listener* l);

	// TODO: these should be part of model transform, not view
	float yaw;
	float pitch;
	gl::Vec3 center;

private:
	gl::Mat4 view;
	gl::Mat4 viewInverse;
	gl::Mat4 projection;
	gl::Vec4 eye;
	gl::Vec4 up;
	gl::Vec4 right;
	gl::Vec4 forward;

	std::vector<Listener*> listeners;
    
    void update();
	void fireEvent();
};

#endif // CGL_CAMERA_H_
