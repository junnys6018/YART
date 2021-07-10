#pragma once
#include "textures/texture.h"

namespace yart
{
	template <typename T>
	class ConstantTexture : public Texture<T>
	{
	public:
		ConstantTexture(const T& value) : m_Value(value)
		{
		}

		virtual T Evaluate(const SurfaceInteraction& surfaceInteraction) const override
		{
			return m_Value;
		}

	private:
		T m_Value;
	};
}