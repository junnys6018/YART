#pragma once
#include "pch.h"

namespace yart
{
	typedef float Float;
	static constexpr Float MaxFloat = std::numeric_limits<Float>::max();
	static constexpr Float Infinity = std::numeric_limits<Float>::infinity();

	void Initialize();
}
