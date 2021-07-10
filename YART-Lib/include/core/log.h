#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>

#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include "core/yart.h"

namespace yart
{
    class Log
    {
    public:
        static void Initialize();

        static Ref<spdlog::logger> GetLogger()
        {
            return s_Logger;
        }

    private:
        static Ref<spdlog::logger> s_Logger;
    };
}

// log macros
#define LOG_TRACE(...) ::yart::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::yart::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::yart::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::yart::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::yart::Log::GetLogger()->critical(__VA_ARGS__)