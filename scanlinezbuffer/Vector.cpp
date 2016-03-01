#include "Vector.h"
Vector::Vector(float xx, float yy, float zz):x(xx),y(yy),z(zz)
{}
Vector::~Vector()
{}
Vector Vector::operator+(const Vector &v) const
{
	return Vector(x + v.x, y + v.y, z + v.z);
}
Vector Vector::operator-(const Vector &v) const
{
	return Vector(x - v.x, y - v.y, z - v.z);
}
Vector Vector::operator*(const float f) const
{
	return Vector(x*f, y*f, z*f);
}
Vector Vector::operator/(const float f) const
{
	float invF = 1.0f / f;
	return Vector(x*invF, y*invF, z*invF);
}
float Vector::Dot(const Vector &v1, const Vector &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
Vector Vector::Cross(const Vector &v1, const Vector &v2)
{
	return Vector(
		v1.y*v2.z - v2.y*v1.z,
		v2.x*v1.z - v1.x*v2.z,
		v1.x*v2.y - v2.x*v1.y);
}