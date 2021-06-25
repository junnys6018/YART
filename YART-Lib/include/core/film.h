#pragma once
#include "math/vector.h"

namespace yart
{
	class Film
	{
	public:
		Film(const Vector2i& resolution) : m_Resolution(resolution)
		{
		}

		Vector2i m_Resolution;
	};
}