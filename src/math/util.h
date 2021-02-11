#pragma once

namespace yart
{
	template <typename T>
	T Lerp(T t, T a, T b)
	{
		return (1 - t) * a + t * b;
	}
}