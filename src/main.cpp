#include "pch.h"

using namespace yart;

int main(int argc, char** argv)
{
	yart::Initialize();
	
	Transform t = LookAt({0, 0, 0}, {3, 3, 3}, {2, 2, 2});
	std::cin.get();
	return 0;
}