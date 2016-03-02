#include "Camera.h"
#include "Point.h"
#include "Vector.h"

Camera::Camera()
{
}

void Camera::LookAt(const Point position, const Vector & gaze, const Vector & up)
{
	Vector eRight = Vector::Normalize(Vector::Cross(gaze, up));
	Vector eUp = Vector::Normalize(Vector::Cross(eRight, gaze));
	Vector eGaze = -Vector::Normalize(gaze);
	m_view.SetMatrix(
		eRight.x, eUp.x, eGaze.x, -position.x,
		eRight.y, eUp.y, eGaze.y, -position.y,
		eRight.z, eUp.z, eGaze.z, -position.z,
		0, 0, 0, 1);
}

void Camera::Perspective(const float zNear, const float zFar, const float aspect, const float fov)
{
}
