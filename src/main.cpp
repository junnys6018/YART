#include "pch.h"

using namespace yart;

int main(int argc, char** argv)
{
	yart::Initialize();
	
	Bounds2i b({0, 0}, {2, 3});

	for (Vector2i p : b)
	{
		LOG_INFO("{}", p);
	}

	std::cin.get();
	return 0;
}