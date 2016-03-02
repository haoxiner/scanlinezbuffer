#pragma once
class Point
{
public:
	Point(const float xx, const float yy, const float zz);
	~Point();
	//Point operator+(const Vector)
public:
	float x, y, z;
};