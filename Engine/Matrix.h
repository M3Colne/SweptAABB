#pragma once

#include "Vec3.h"

template <typename T, size_t S>
class _Mat
{
public:
	_Mat& operator=(const _Mat& rhs)
	{
		memcpy(elements, rhs.elements, sizeof(elements));
		return *this;
	}
	_Mat& operator*=(T rhs)
	{
		for (auto& row : elements)
		{
			for (T& e : row)
			{
				e *= rhs;
			}
		}
		return *this;
	}
	_Mat operator*(T rhs) const
	{
		_Mat result = *this;
		return result *= rhs;
	}
	_Mat& operator*=(const _Mat& rhs)
	{
		return *this = *this * rhs;
	}
	_Mat operator*(const _Mat& rhs) const
	{
		_Mat result = {};
		for (size_t j = 0; j < S; j++)
		{
			for (size_t k = 0; k < S; k++)
			{
				for (size_t i = 0; i < S; i++)
				{
					result.elements[j][k] += elements[j][i] * rhs.elements[i][k];
				}
			}
		}
		return result;
	}
	_Mat operator!() const
	{
		_Mat xp;
		for (size_t j = 0; j < S; j++)
		{
			for (size_t k = 0; k < S; k++)
			{
				xp.elements[j][k] = elements[k][j];
			}
		}
		return xp;
	}
	constexpr static _Mat Identity()
	{
		if constexpr (S == 3)
		{
			return {
				(T)1.0,(T)0.0,(T)0.0,
				(T)0.0,(T)1.0,(T)0.0,
				(T)0.0,(T)0.0,(T)1.0,
			};
		}
		else if constexpr (S == 4)
		{
			return {
				(T)1.0,(T)0.0,(T)0.0,(T)0.0,
				(T)0.0,(T)1.0,(T)0.0,(T)0.0,
				(T)0.0,(T)0.0,(T)1.0,(T)0.0,
				(T)0.0,(T)0.0,(T)0.0,(T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	constexpr static _Mat Scaling(T factor)
	{
		if constexpr (S == 3)
		{
			return{
				factor,(T)0.0,(T)0.0,
				(T)0.0,factor,(T)0.0,
				(T)0.0,(T)0.0,factor,
			};
		}
		else if constexpr (S == 4)
		{
			return {
				factor,(T)0.0,(T)0.0,(T)0.0,
				(T)0.0,factor,(T)0.0,(T)0.0,
				(T)0.0,(T)0.0,factor,(T)0.0,
				(T)0.0,(T)0.0,(T)0.0,(T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}

	}
	static _Mat RotationZ(T theta)
	{
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		if constexpr (S == 3)
		{
			return{
				 cosTheta, sinTheta, (T)0.0,
				-sinTheta, cosTheta, (T)0.0,
				(T)0.0,    (T)0.0,   (T)1.0,
			};
		}
		else if constexpr (S == 4)
		{
			return {
				 cosTheta, sinTheta, (T)0.0,(T)0.0,
				-sinTheta, cosTheta, (T)0.0,(T)0.0,
				(T)0.0,    (T)0.0,   (T)1.0,(T)0.0,
				(T)0.0,	   (T)0.0,   (T)0.0,(T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	static _Mat RotationY(T theta)
	{
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		if constexpr (S == 3)
		{
			return{
				 cosTheta, (T)0.0,-sinTheta,
				 (T)0.0,   (T)1.0, (T)0.0,
				 sinTheta, (T)0.0, cosTheta
			};
		}
		else if constexpr (S == 4)
		{
			return {
				cosTheta, (T)0.0, -sinTheta,(T)0.0,
				(T)0.0,   (T)1.0, (T)0.0,   (T)0.0,
				sinTheta, (T)0.0, cosTheta, (T)0.0,
				(T)0.0,   (T)0.0, (T)0.0,   (T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	static _Mat RotationX(T theta)
	{
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		if constexpr (S == 3)
		{
			return{
				(T)1.0, (T)0.0,   (T)0.0,
				(T)0.0, cosTheta, sinTheta,
				(T)0.0,-sinTheta, cosTheta,
			};
		}
		else if constexpr (S == 4)
		{
			return {
				(T)1.0, (T)0.0,   (T)0.0,  (T)0.0,
				(T)0.0, cosTheta, sinTheta,(T)0.0,
				(T)0.0,-sinTheta, cosTheta,(T)0.0,
				(T)0.0, (T)0.0,   (T)0.0,  (T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	template<class V>
	constexpr static _Mat Translation(const V& tl)
	{
		return Translation(tl.x, tl.y, tl.z);
	}
	constexpr static _Mat Translation(T x, T y, T z)
	{
		if constexpr (S == 4)
		{
			return {
				(T)1.0,(T)0.0,(T)0.0,(T)0.0,
				(T)0.0,(T)1.0,(T)0.0,(T)0.0,
				(T)0.0,(T)0.0,(T)1.0,(T)0.0,
				x,     y,      z,    (T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	constexpr static _Mat Projection(T w, T h, T n, T f)
	{
		if constexpr (S == 4)
		{
			return {
				(T)2.0 * n / w,	(T)0.0,			(T)0.0,				(T)0.0,
				(T)0.0,			(T)2.0 * n / h,	(T)0.0,				(T)0.0,
				(T)0.0,			(T)0.0,			f / (f - n),		(T)1.0,
				(T)0.0,			(T)0.0,			-n * f / (f - n),	(T)0.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	constexpr static _Mat ProjectionHFOV(T fov, T ar, T n, T f)
	{
		if constexpr (S == 4)
		{
			const auto fov_rad = fov * (T)PI / (T)180.0;
			const auto w = (T)1.0f / std::tan(fov_rad / (T)2.0);
			const auto h = w * ar;
			return {
				w,		(T)0.0,	(T)0.0,				(T)0.0,
				(T)0.0,	h,		(T)0.0,				(T)0.0,
				(T)0.0,	(T)0.0,	f / (f - n),		(T)1.0,
				(T)0.0,	(T)0.0,	-n * f / (f - n),	(T)0.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
public:
	// [ row ][ col ]
	T elements[S][S];
};

typedef _Mat<float, 3> Mat3;
typedef _Mat<double, 3> Mad3;
typedef _Mat<float, 4> Mat4;
typedef _Mat<double, 4> Mad4;