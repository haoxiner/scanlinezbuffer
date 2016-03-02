#pragma once
#include "Vector.h"
class Point
{
public:
	Point(const float xx, const float yy, const float zz);
	~Point();
	//Point operator+(const Vector)
	Vector operator-(const Point &p) const;
public:
	float x, y, z;
};