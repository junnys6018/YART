#pragma once
#include "filter.h"

namespace yart
{
    class TriangleFilter final : public Filter
    {
    public:
        TriangleFilter(const Vector2f& radius) : Filter(radius)
        {
        }

        virtual real Evaluate(const Vector2f& point) const override
        {
            return std::max((real)0, m_Radius.x - std::abs(point.x)) * std::max((real)0, m_Radius.y - std::abs(point.y));
        }
    };
}