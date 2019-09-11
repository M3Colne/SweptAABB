#include "Vec2.h"
#include <cmath>

Vec2::Vec2(float x_in, float y_in)
	:
	x(x_in),
	y(y_in)
{
}

Vec2& Vec2::operator=(const Vec2& rhs)
{
	x = rhs.x;
	y = rhs.y;

	return *this;
}
Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2& Vec2::operator+=(const Vec2& rhs)
{
	return *this = *this + rhs;
}

Vec2 Vec2::operator*(float rhs) const
{
	return Vec2(x * rhs, y * rhs);
}

Vec2& Vec2::operator*=(float rhs)
{
	return *this = *this * rhs;
}

Vec2 Vec2::operator/(float rhs) const
{
	return Vec2(x / rhs, y / rhs);
}

Vec2& Vec2::operator/=(float rhs)
{
	return *this = *this / rhs;
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2& Vec2::operator-=(const Vec2& rhs)
{
	return *this = *this - rhs;
}

float Vec2::GetLength() const
{
	return std::sqrt(GetLengthSq());
}

float Vec2::GetLengthSq() const
{
	return x * x + y * y;
}

Vec2& Vec2::Normalize()
{
	return *this = GetNormalized();
}

Vec2 Vec2::GetNormalized() const
{
	const float len = GetLength();
	if (len != 0.0f)
	{
		return *this * (1.0f / len);
	}
	return *this;
}

Vec2& Vec2::NormalizeTo(float normalizer)
{
	return *this = GetNormalizedTo(normalizer);
}

Vec2 Vec2::GetNormalizedTo(float normalizer) const
{
	const float len = GetLength();
	if (len != 0.0f)
	{
		return *this * (normalizer / len);
	}
	return *this;
}

float Vec2::GetAngle(const float relativeToWhat) const
{
	//RelativeToWhat must be between 0 and 2*PI
	//The return value can be between -PI and +PI
	const float PI = 3.1415926f;
	Vec2 a = this->GetNormalized();
	Vec2 r(float(cos(relativeToWhat)), float(sin(relativeToWhat)));

	//Finding the angle relative to 0
	float absAngle = 0.0f;
	if (relativeToWhat <= PI)
	{
		absAngle = GetAngleBetween(a, Vec2(1.0f, 0.0f));
		if (a.y < 0)
		{
			absAngle = 2 * PI - absAngle;
		}
	}
	else
	{
		absAngle = GetAngleBetween(a, Vec2(1.0f, 0.0f));
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

float Vec2::GetAngle() const
{
	if (this->y < 0)
	{
		return 6.2831853f - acos(this->GetNormalized().x);
	}

	return acos(this->GetNormalized().x);
}

float Vec2::GetAngleBetween(const Vec2 a, const Vec2 b)
{
	//It will be between 0 and PI and never negative
	//acos((this->x * b.x + this->y * b.y) / (this->GetLength() * b.GetLength()));
	//Dot product
	float dp = a.x * b.x + a.y * b.y;
	float u = a.GetLength();
	float v = b.GetLength();

	float beta = dp / (u * v);

	return acos(beta);
}
