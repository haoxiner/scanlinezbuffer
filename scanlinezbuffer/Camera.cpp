#include "Camera.h"
#include "Point.h"
#include "Vector.h"

Camera::Camera()
{
}

Camera::Camera(float xResolution, float yResolution):
	m_xResolution(xResolution),m_yResolution(yResolution)
{
}

void Camera::LookAt(const Point position, const Vector & gaze, const Vector & up)
{
	// right-hand coordinate
	Vector eRight = Vector::Normalize(Vector::Cross(gaze, up));
	Vector eUp = Vector::Normalize(Vector::Cross(eRight, gaze));
	Vector eGaze = -Vector::Normalize(gaze);

	Transform translate = Transform::Translate(-position.x, -position.y, -position.z);
	Transform rotate(
		eRight.x, eUp.x, eGaze.x, 0,
		eRight.y, eUp.y, eGaze.y, 0,
		eRight.z, eUp.z, eGaze.z, 0,
		0, 0, 0, 1);
	m_view = rotate*translate;
}

void Camera::Frustum(const float l, const float r, const float t, const float b, const float n, const float f)
{
	float w = r - l;
	float h = t - b;
	float depth = n - f;
	float doubleN = 2 * n;
	m_proj.SetMatrix(
		doubleN / w, 0, (r + l) / w, 0,
		0, doubleN / h, (t + b) / h, 0,
		0, 0, (f + n) / depth, doubleN*f / depth,
		0, 0, -1, 0
		);
}

Transform Camera::WorldToProjection() const
{
	return m_proj*m_view;
}

