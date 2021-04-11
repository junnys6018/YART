#pragma once
#include "pch.h"

namespace yart
{
#if defined(USE_DOUBLE_PRECISION_FLOAT)
	typedef double real;
#else
	typedef float real;
#endif

	static constexpr real MaxFloat = std::numeric_limits<real>::max();
	static constexpr real Infinity = std::numeric_limits<real>::infinity();

	void Initialize();
}
