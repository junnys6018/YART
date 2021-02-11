#pragma once
#include "pch.h"

namespace yart
{
#if defined(USE_DOUBLE_PRECISION_FLOAT)
	typedef double Float;
#else
	typedef float Float;
#endif

	static constexpr Float MaxFloat = std::numeric_limits<Float>::max();
	static constexpr Float Infinity = std::numeric_limits<Float>::infinity();

	void Initialize();
}
