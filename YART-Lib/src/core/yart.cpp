#include "core/yart.h"
#include "core/spectrum.h"

namespace yart
{
	void Initialize()
	{
		Log::Initialize();
		SampledSpectrum::Initialize();
	}
}