#include "pch.h"

using namespace yart;

int main(int argc, char** argv)
{
	yart::Initialize();

	Transform t = RotateX(45);
	Bounds3f bb({0, 0, 0}, {10, 10, 10});

	LOG_INFO("{}", t.AppBB(bb));

	std::cin.get();
	return 0;
}