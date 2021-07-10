#pragma once
#include "core/assert.h"

namespace yart
{
    using i8 = int8_t;
    using u8 = uint8_t;
    using i16 = int16_t;
    using u16 = uint16_t;
    using i32 = int32_t;
    using u32 = uint32_t;
    using i64 = int64_t;
    using u64 = uint64_t;

#if defined(USE_DOUBLE_PRECISION_FLOAT)
    using real = double;
    using ureal = u64;
#else
    using real = float;
    using ureal = u32;
#endif

#ifndef YART_L1_CACHE_SIZE
#define YART_L1_CACHE_SIZE 64
#endif

#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX)
#error Unknown platform
#endif

#if !defined(CONFIGURATION_DEBUG) && !defined(CONFIGURATION_RELEASE)
#error Unknown configuration
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
