#include "Point.h"
Point::Point():x(0),y(0),z(0){}
Point::Point(const float xx, const float yy, const float zz) :x(xx), y(yy), z(zz)
{}
Point::~Point()
{}

Vector Point::operator-(const Point & p) const
{
	return Vector(x - p.x, y - p.y, z - p.z);
}

bool Point::operator<(const Point & p) const
{
	return y < p.y;
}
