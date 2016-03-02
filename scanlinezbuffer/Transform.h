#pragma once
#include "Vector.h"
#include "Point.h"
class Transform
{
public:
	Transform();
	Transform(
		float, float, float, float,
		float, float, float, float,
		float, float, float, float,
		float, float, float, float);
	void SetMatrix(
		float, float, float, float,
		float, float, float, float,
		float, float, float, float,
		float, float, float, float);
	Point operator()(const Point &p) const;
	Vector operator()(const Vector &v) const;
	Transform operator*(const Transform &t) const;
	Vector operator*(const Vector &v) const;
	Point operator*(const Point &p) const;
private:
	float m_matrix[4][4];
};