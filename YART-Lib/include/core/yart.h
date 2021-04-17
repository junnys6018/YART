#pragma once

namespace yart
{
#if defined(USE_DOUBLE_PRECISION_FLOAT)
	typedef double real;
	typedef uint64_t uint;
	#define YART_POSITIVE_ZERO 0.0
	#define YART_NEGATIVE_ZERO -0.0
#else
	typedef float real;
	typedef uint32_t uint;
	#define YART_POSITIVE_ZERO 0.0f
	#define YART_NEGATIVE_ZERO -0.0f
#endif

	void Initialize();
}
