#pragma once

#include <cmath>

template<typename T>
class Vec3_
{
public:
	Vec3_(T x_in, T y_in, T z_in)
		:
		x(x_in),
		y(y_in),
		z(z_in)
	{
	}
	Vec3_& operator=(const Vec3_& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;

		return *this;
	}
	Vec3_ operator+(const Vec3_& rhs) const
	{
		return Vec3_(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	Vec3_& operator+=(const Vec3_& rhs)
	{
		return *this = *this + rhs;
	}
	Vec3_ operator*(T rhs) const
	{
		return Vec3_(x * rhs, y * rhs, z * rhs);
	}
	Vec3_& operator*=(T rhs)
	{
		return *this = *this * rhs;
	}
	Vec3_ operator/(T rhs) const
	{
		return Vec3_(x / rhs, y / rhs, z / rhs);
	}
	Vec3_& operator/=(T rhs)
	{
		return *this = *this / rhs;
	}
	Vec3_ operator-(const Vec3_& rhs) const
	{
		return Vec3_(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	Vec3_& operator-=(const Vec3_& rhs)
	{
		return *this = *this - rhs;
	}
	T GetLength() const
	{
		return T(std::sqrt(GetLengthSq()));
	}
	T GetLengthSq() const
	{
		return x * x + y * y + z * z;
	}
	Vec3_& Normalize()
	{
		return *this = GetNormalized();
	}
	Vec3_ GetNormalized() const
	{
		const T len = GetLength();
		if (len != T(0))
		{
			return *this * (T(1) / len);
		}
		return *this;
	}
	static float DotProduct(const Vec3_& a, const Vec3_& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
	static Vec3_ CrossProduct(const Vec3_& a, const Vec3_& b)
	{
		return Vec3_(a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
	}
public:
	T x;
	T y;
	T z;
};