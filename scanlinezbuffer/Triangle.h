#pragma once
#include "Point.h"
class Triangle
{
public:
	Triangle(
		float x0, float y0, float z0,
		float x1, float y1, float z1,
		float x2, float y2, float z2);
	//~Triangle();
	// p0.y <= p1.y <= p2.y
	Point p0, p1, p2;
	bool operator<(const Triangle &t)
	{
		return p0.y < t.p0.y;
	}
};
