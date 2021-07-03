#include "core/film.h"
#include "core/yart.h"

namespace yart
{
	Film::~Film()
	{
		FreeAligned(m_Pixels);
	}

	Bounds2i Film::GetSampleBounds() const
	{
		Bounds2f sampleBounds{Floor(Vector2f(m_CroppedPixelBounds.m_MinBound) + Vector2f{0.5, 0.5} - m_Filter->m_Radius),
							  Ceil(Vector2f(m_CroppedPixelBounds.m_MaxBound) - Vector2f{0.5, 0.5} + m_Filter->m_Radius)};
		return Bounds2i(sampleBounds);
	}

	Bounds2f Film::GetPhysicalExtent() const
	{
		real aspect = (real)m_Resolution.y / (real)m_Resolution.x;
		real x = std::sqrt(m_PhysicalDiagonal * m_PhysicalDiagonal / (1 + aspect * aspect));
		real y = aspect * x;
		return Bounds2f{{-x / 2, -y / 2}, {x / 2, y / 2}};
	}

	void Film::WriteImage() const
	{
		Scope<real[]> pixelData(new real[3 * m_CroppedPixelBounds.Area()]);
		i32 offset = 0;
		for (Vector2i point : m_CroppedPixelBounds)
		{
			const Pixel& pixel = GetPixel(point);
			std::array<real, 3> xyz;
			xyz[0] = pixel.xyz[0];
			xyz[1] = pixel.xyz[1];
			xyz[2] = pixel.xyz[2];
			std::array<real, 3> rgb = XYZToRGB(xyz);
			pixelData[3 * offset + 0] = rgb[0];
			pixelData[3 * offset + 1] = rgb[1];
			pixelData[3 * offset + 2] = rgb[2];

			real filterWeightSum = pixel.FilterWeightSum;
			if (filterWeightSum != 0)
			{
				real invWt = 1 / filterWeightSum;
				pixelData[3 * offset + 0] = std::max((real)0, pixelData[3 * offset + 0] * invWt);
				pixelData[3 * offset + 1] = std::max((real)0, pixelData[3 * offset + 1] * invWt);
				pixelData[3 * offset + 2] = std::max((real)0, pixelData[3 * offset + 2] * invWt);
			}
			offset++;
		}
	}
}