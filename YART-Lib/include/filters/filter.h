#pragma once
#include "math/vector.h"

namespace yart
{
	class Filter
	{
	public:
		Filter(const Vector2f& radius) : m_Radius(radius), m_InvRadius(1 / radius.x, 1 / radius.y)
		{
		}

		virtual real Evaluate(const Vector2f& point) const = 0;

		const Vector2f m_Radius, m_InvRadius;
	};
}