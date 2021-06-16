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

	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
