#pragma once

namespace yart
{
#if defined(USE_DOUBLE_PRECISION_FLOAT)
	typedef double real;
#else
	typedef float real;
#endif

#ifndef YART_L1_CACHE_SIZE
#define YART_L1_CACHE_SIZE 64
#endif

#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX)
	#error Unknown Platform!
#endif

	void Initialize();
}
