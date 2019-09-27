#pragma once

#include <cmath>

template<typename T>
class Vec2_
{
public:
	Vec2_(T x_in, T y_in)
		:
		x(x_in),
		y(y_in)
	{
	}
	Vec2_& operator=(const Vec2_& rhs)
	{
		x = rhs.x;
		y = rhs.y;

		return *this;
	}
	Vec2_ operator+(const Vec2_& rhs) const
	{
		return Vec2_(x + rhs.x, y + rhs.y);
	}
	Vec2_& operator+=(const Vec2_& rhs)
	{
		return *this = *this + rhs;
	}
	Vec2_ operator*(T rhs) const
	{
		return Vec2_(x * rhs, y * rhs);
	}
	Vec2_& operator*=(T rhs)
	{
		return *this = *this * rhs;
	}
	Vec2_ operator/(T rhs) const
	{
		return Vec2_(x / rhs, y / rhs);
	}
	Vec2_& operator/=(T rhs)
	{
		return *this = *this / rhs;
	}
	Vec2_ operator-(const Vec2_& rhs) const
	{
		return Vec2_(x - rhs.x, y - rhs.y);
	}
	Vec2_& operator-=(const Vec2_& rhs)
	{
		return *this = *this - rhs;
	}
	T GetLength() const
	{
		return T(std::sqrt(GetLengthSq()));
	}
	T GetLengthSq() const
	{
		return x * x + y * y;
	}
	Vec2_& Normalize()
	{
		return *this = GetNormalized();
	}
	Vec2_ GetNormalized() const
	{
		const T len = GetLength();
		if (len != T(0))
		{
			return *this * (T(1) / len);
		}
		return *this;
	}
	Vec2_& NormalizeTo(T normalizer)
	{
		return *this = GetNormalizedTo(normalizer);
	}
	Vec2_ GetNormalizedTo(T normalizer) const
	{
		const T len = GetLength();
		if (len != T(0))
		{
			return *this * (normalizer / len);
		}
		return *this;
	}
	float GetAngle(const float relativeToWhat) const
	{
		//RelativeToWhat must be between 0 and 2*PI
		//The return value can be between -PI and +PI
		const float PI = 3.1415926f;
		Vec2_<float> a = this->GetNormalized();
		Vec2_<float> r(float(cos(relativeToWhat)), float(sin(relativeToWhat)));

		//Finding the angle relative to 0
		float absAngle = 0.0f;
		if (relativeToWhat <= PI)
		{
			absAngle = GetAngleBetween(a, Vec2_<float>(1.0f, 0.0f));
			if (a.y < 0)
			{
				absAngle = 2 * PI - absAngle;
			}
		}
		else
		{
			absAngle = GetAngleBetween(a, Vec2_<float>(1.0f, 0.0f));
			if (a.y < 0)
			{
				absAngle = 2 * PI - absAngle;
			}
			else
			{
				absAngle += 2 * PI;
			}
		}

		if (absAngle >= relativeToWhat && absAngle <= relativeToWhat + PI)
		{
			return GetAngleBetween(a, r);
		}
		else
		{
			return -GetAngleBetween(a, r);
		}
	}
	float GetAngle() const
	{
		if (this->y < 0)
		{
			return 6.2831853f - acos(this->GetNormalized().x);
		}

		return acos(this->GetNormalized().x);
	}
	float GetAngleBetween(const Vec2_ a, const Vec2_ b)
	{
		//It will be between 0 and PI and never negative
		//acos((this->x * b.x + this->y * b.y) / (this->GetLength() * b.GetLength()));
		//Dot product
		T dp = a.x * b.x + a.y * b.y;
		T u = a.GetLength();
		T v = b.GetLength();

		T beta = dp / (u * v);

		return acos(beta);
	}
public:
	T x;
	T y;
};

typedef Vec2_<float> Vec2;
typedef Vec2_<int> Vei2;