#pragma once
class Point;
class Vector;
class Transform
{
public:
	Transform(
		float, float, float, float,
		float, float, float, float,
		float, float, float, float,
		float, float, float, float);
	Point operator()(const Point &p) const;
	Vector operator()(const Vector &v) const;
private:
	float m_matrix[4][4];
};