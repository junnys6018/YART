#pragma once
#include "filter.h"

namespace yart
{
    class BoxFilter final : public Filter
    {
    public:
        BoxFilter(const Vector2f& radius) : Filter(radius)
        {
        }

        virtual real Evaluate(const Vector2f& point) const override
        {
            return 1;
        }
    };
}