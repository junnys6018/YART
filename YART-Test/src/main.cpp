#define CATCH_CONFIG_RUNNER
#include <catch_amalgamated.hpp>
#include "yart.h"
using namespace yart;

int main(int argc, char* argv[])
{
	yart::Initialize();

	int result = Catch::Session().run(argc, argv);
	return result;
}