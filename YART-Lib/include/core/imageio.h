#pragma once
#include "math/boundingbox.h"

namespace yart
{
	void WriteImage(const std::string& filename, const real* pixels, const Bounds2i& outputBounds,
					const Vector2i& totalResolution);
}