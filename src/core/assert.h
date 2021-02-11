#pragma once

#if defined(CONFIGURATION_DEBUG)

	#if defined(PLATFORM_WINDOWS)
		#define YART_DEBUGBREAK() __debugbreak()
	#elif defined(PLATFORM_LINUX)
		#include <signal.h>
		#define YART_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error unknown platform
	#endif

	#define ASSERT(x) { if (!(x)) { YART_DEBUGBREAK(); } }

#else
	#define ASSERT(x) (void)0
#endif