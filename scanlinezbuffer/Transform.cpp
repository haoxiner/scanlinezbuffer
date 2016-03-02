#include "Transform.h"
#include <memory>
Transform::Transform()
{
	memset(m_matrix, 0, sizeof(float) * 16);
}
Transform::Transform(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	m_matrix[0][0] = m00;m_matrix[0][1] = m01;m_matrix[0][2] = m02;m_matrix[0][3] = m03;
	m_matrix[1][0] = m10;m_matrix[1][1] = m11;m_matrix[1][2] = m12;m_matrix[1][3] = m13;
	m_matrix[2][0] = m20;m_matrix[2][1] = m21;m_matrix[2][2] = m22;m_matrix[2][3] = m23;
	m_matrix[3][0] = m30;m_matrix[3][1] = m31;m_matrix[3][2] = m32;m_matrix[3][3] = m33;
}
void Transform::SetMatrix(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	m_matrix[0][0] = m00;m_matrix[0][1] = m01;m_matrix[0][2] = m02;m_matrix[0][3] = m03;
	m_matrix[1][0] = m10;m_matrix[1][1] = m11;m_matrix[1][2] = m12;m_matrix[1][3] = m13;
	m_matrix[2][0] = m20;m_matrix[2][1] = m21;m_matrix[2][2] = m22;m_matrix[2][3] = m23;
	m_matrix[3][0] = m30;m_matrix[3][1] = m31;m_matrix[3][2] = m32;m_matrix[3][3] = m33;
}
Point Transform::operator()(const Point &p) const
{
	float w = m_matrix[3][0] * p.x + m_matrix[3][1] * p.y + m_matrix[3][2] * p.z + m_matrix[3][3];
	if (w != 0.0f)
	{
		float invW = 1.0f / w;
		return Point(
			invW*(m_matrix[0][0] * p.x + m_matrix[0][1] * p.y + m_matrix[0][2] * p.z + m_matrix[0][3]),
			invW*(m_matrix[1][0] * p.x + m_matrix[1][1] * p.y + m_matrix[1][2] * p.z + m_matrix[1][3]),
			invW*(m_matrix[2][0] * p.x + m_matrix[2][1] * p.y + m_matrix[2][2] * p.z + m_matrix[2][3]));
	}
	return Point(
		m_matrix[0][0] * p.x + m_matrix[0][1] * p.y + m_matrix[0][2] * p.z + m_matrix[0][3],
		m_matrix[1][0] * p.x + m_matrix[1][1] * p.y + m_matrix[1][2] * p.z + m_matrix[1][3],
		m_matrix[2][0] * p.x + m_matrix[2][1] * p.y + m_matrix[2][2] * p.z + m_matrix[2][3]);
}
Vector Transform::operator()(const Vector &v) const
{
	return Vector(
		m_matrix[0][0] * v.x + m_matrix[0][1] * v.y + m_matrix[0][2] * v.z + m_matrix[0][3],
		m_matrix[1][0] * v.x + m_matrix[1][1] * v.y + m_matrix[1][2] * v.z + m_matrix[1][3],
		m_matrix[2][0] * v.x + m_matrix[2][1] * v.y + m_matrix[2][2] * v.z + m_matrix[2][3]);
}

Transform Transform::operator*(const Transform & t) const
{
	Transform ret;
	for (size_t i = 0; i < 4; ++i)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			ret.m_matrix[i][j] =
				m_matrix[i][0] * t.m_matrix[0][j] +
				m_matrix[i][1] * t.m_matrix[1][j] +
				m_matrix[i][2] * t.m_matrix[2][j] +
				m_matrix[i][3] * t.m_matrix[3][j];
		}
	}
	return ret;
}

Vector Transform::operator*(const Vector & v) const
{
	return Vector(
		m_matrix[0][0] * v.x + m_matrix[0][1] * v.y + m_matrix[0][2] * v.z + m_matrix[0][3],
		m_matrix[1][0] * v.x + m_matrix[1][1] * v.y + m_matrix[1][2] * v.z + m_matrix[1][3],
		m_matrix[2][0] * v.x + m_matrix[2][1] * v.y + m_matrix[2][2] * v.z + m_matrix[2][3]);
}

Point Transform::operator*(const Point & p) const
{
	float w = m_matrix[3][0] * p.x + m_matrix[3][1] * p.y + m_matrix[3][2] * p.z + m_matrix[3][3];
	if (w != 0.0f)
	{
		float invW = 1.0f / w;
		return Point(
			invW*(m_matrix[0][0] * p.x + m_matrix[0][1] * p.y + m_matrix[0][2] * p.z + m_matrix[0][3]),
			invW*(m_matrix[1][0] * p.x + m_matrix[1][1] * p.y + m_matrix[1][2] * p.z + m_matrix[1][3]),
			invW*(m_matrix[2][0] * p.x + m_matrix[2][1] * p.y + m_matrix[2][2] * p.z + m_matrix[2][3]));
	}
	return Point(
		m_matrix[0][0] * p.x + m_matrix[0][1] * p.y + m_matrix[0][2] * p.z + m_matrix[0][3],
		m_matrix[1][0] * p.x + m_matrix[1][1] * p.y + m_matrix[1][2] * p.z + m_matrix[1][3],
		m_matrix[2][0] * p.x + m_matrix[2][1] * p.y + m_matrix[2][2] * p.z + m_matrix[2][3]);
}
