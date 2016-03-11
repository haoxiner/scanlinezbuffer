#include "Triangle.h"
#include <algorithm>
Triangle::Triangle(
	float x0, float y0, float z0,
	float x1, float y1, float z1,
	float x2, float y2, float z2)
{
	p[0].x = x0;
	p[0].y = y0;
	p[0].z = z0;

	p[1].x = x1;
	p[1].y = y1;
	p[1].z = z1;

	p[2].x = x2;
	p[2].y = y2;
	p[2].z = z2;
}

Triangle::Triangle(const Point & p0, const Point & p1, const Point & p2)
{
	p[0] = p0;
	p[1] = p1;
	p[2] = p2;
}

void Triangle::SortVertex()
{
	std::sort(p, p + 3);
}
