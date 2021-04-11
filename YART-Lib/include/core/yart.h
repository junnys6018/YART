#pragma once

namespace yart
{
#if defined(USE_DOUBLE_PRECISION_FLOAT)
	typedef double real;
#else
	typedef float real;
#endif

	void Initialize();
}
