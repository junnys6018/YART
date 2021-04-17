#pragma once
#include "core/yart.h"

namespace yart
{
// Disable double to float truncation warning
#pragma warning(disable : 4305)
	static constexpr real ShadowEpsilon = 0.0001;
	static constexpr real MachineEpsilon = std::numeric_limits<real>::epsilon() * 0.5;
	static constexpr real Epsilon = std::numeric_limits<real>::epsilon();
	static constexpr real Pi = 3.14159265358979323846;
	static constexpr real InvPi = 0.31830988618379067154;
	static constexpr real Inv2Pi = 0.15915494309189533577;
	static constexpr real Inv4Pi = 0.07957747154594766788;
	static constexpr real PiOver2 = 1.57079632679489661923;
	static constexpr real PiOver4 = 0.78539816339744830961;
	static constexpr real Sqrt2 = 1.41421356237309504880;
	static constexpr real MaxFloat = std::numeric_limits<real>::max();
	static constexpr real Infinity = std::numeric_limits<real>::infinity();
#pragma warning(default : 4305)

	template <typename T>
	constexpr inline T Lerp(T t, T a, T b)
	{
		return (1 - t) * a + t * b;
	}

	constexpr inline real Radians(real deg)
	{
		return (Pi / 180) * deg;
	}

	constexpr inline real gamma(int n)
	{
		return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
	}

	template <typename T>
	constexpr inline T Clamp(T val, T min, T max)
	{
		if (val < min)
			return min;
		if (val > max)
			return max;
		return val;
	}

	inline uint RealToBits(real v)
	{
		uint ui;
		std::memcpy(&ui, &v, sizeof(real));
		return ui;
	}

	inline real BitsToReal(uint bits)
	{
		real v;
		std::memcpy(&v, &bits, sizeof(real));
		return v;
	}

	inline real NextFloatUp(real v)
	{
		if (std::isinf(v) && v > 0)
		{
			return v;
		}
		if (v == YART_NEGATIVE_ZERO)
		{
			return YART_POSITIVE_ZERO;
		}

		uint ui = RealToBits(v);
		if (v >= 0)
			ui++;
		else
			ui--;

		return BitsToReal(ui);
	}

	inline real NextFloatDown(real v)
	{
		if (std::isinf(v) && v < 0)
		{
			return v;
		}
		if (v == YART_POSITIVE_ZERO)
		{
			return YART_NEGATIVE_ZERO;
		}

		uint ui = RealToBits(v);
		if (v >= 0)
			ui--;
		else
			ui++;

		return BitsToReal(ui);
	}

	// Solves the quadratic equation a*t^2 + b*t + c = 0, returns false if no solutions exist
	bool Quadratic(real a, real b, real c, real* t1, real* t2);
}