#include "accelerators/bvh.h"

namespace yart
{
    const char* const SplitMethodNames[] = {"Middle", "EqualCounts"};
    static_assert((sizeof(SplitMethodNames) / sizeof(const char*)) == static_cast<i32>(SplitMethod::COUNT));
}