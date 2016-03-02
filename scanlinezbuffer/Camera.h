#pragma once
#include "Transform.h"
class Point;
class Vector;
class Camera
{
public:
	Camera();
	void LookAt(const Point position, const Vector &gaze, const Vector &up);
	void Perspective(const float zNear, const float zFar, const float aspect, const float fov);
private:
	Transform m_proj;
	Transform m_view;
};