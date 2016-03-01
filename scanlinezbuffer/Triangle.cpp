#include "Triangle.h"
Triangle::Triangle(
	float x0, float y0, float z0,
	float x1, float y1, float z1,
	float x2, float y2, float z2) :
	p0(x0, y0, z0), p1(x1, y1, z1), p2(x2, y2, z2)
{}