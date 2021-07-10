#define CATCH_CONFIG_RUNNER
#include "yart.h"
#include <catch_amalgamated.hpp>
using namespace yart;

#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX)
#error Unknown platform
#endif

#if !defined(CONFIGURATION_DEBUG) && !defined(CONFIGURATION_RELEASE)
#error Unknown configuration
#endif

int main(int argc, char* argv[])
{
    std::cout << "Running YART-Test ";
#if defined(PLATFORM_WINDOWS)
    std::cout << "(Windows, ";
#elif defined(PLATFORM_LINUX)
    std::cout << "(Linux, ";
#endif

#if defined(CONFIGURATION_DEBUG)
    std::cout << "Debug)\n";
#elif defined(CONFIGURATION_RELEASE)
    std::cout << "Release)\n";
#endif
    yart::Initialize();

    int result = Catch::Session().run(argc, argv);
    return result;
}