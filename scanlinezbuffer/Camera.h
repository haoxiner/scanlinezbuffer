#pragma once
#include "Transform.h"
class Point;
class Vector;
class Camera
{
public:
	Camera();
	Camera(float xResolution,float yResolution);
	void LookAt(const Point position, const Vector &gaze, const Vector &up);
	void Frustum(const float l, const float r, const float t, const float b, const float n, const float f);
	Transform WorldToProjection()const;
private:
	Transform m_proj;
	Transform m_view;
	float m_xResolution, m_yResolution;
};