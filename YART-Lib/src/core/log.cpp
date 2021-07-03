#include "core/log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace yart
{
	Ref<spdlog::logger> Log::s_Logger;

	void Log::Initialize()
	{
		spdlog::set_pattern("%^[%T] [%n] [%l]: %v%$");
		s_Logger = spdlog::stdout_color_mt("YART");
		s_Logger->set_level(spdlog::level::trace);
	}
}