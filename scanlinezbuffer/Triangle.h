#pragma once
#include "Point.h"
#include <vector>
class Triangle
{
public:
	Triangle(
		float x0, float y0, float z0,
		float x1, float y1, float z1,
		float x2, float y2, float z2);
	Triangle(const Point &p0, const Point &p1, const Point &p2);
	bool Check();
	// p0.y <= p1.y <= p2.y
	float color;
	Point p[3];
	void SortVertex();
	bool operator<(const Triangle &t)
	{
		return p[0].y < t.p[0].y;
	}
};
