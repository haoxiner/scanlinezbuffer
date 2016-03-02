#include "Point.h"
Point::Point(const float xx, const float yy, const float zz) :x(xx), y(yy), z(zz)
{}
Point::~Point()
{}

Vector Point::operator-(const Point & p) const
{
	return Vector(x - p.x, y - p.y, z - p.z);
}
