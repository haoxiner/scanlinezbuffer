#pragma once
class Vector
{
public:
	Vector(float xx, float yy, float zz);
	~Vector();
	Vector operator+(const Vector &v) const;
	Vector operator-(const Vector &V) const;
	Vector operator-() const;
	Vector operator*(const float f) const;
	Vector operator/(const float f) const;
	float Length() const;
	static float Dot(const Vector &v1, const Vector &v2);
	static Vector Cross(const Vector &v1, const Vector &v2);
	static Vector Normalize(const Vector &v);
public:
	float x, y, z;
};