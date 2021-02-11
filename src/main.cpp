#include "pch.h"

#include "math/vector.h"

using namespace yart;

int main(int argc, char** argv)
{
	yart::Initialize();

	
	Vector3f vec{2, 4, 6};

	Vector3f vec2 = vec / 2.0f;

	LOG_INFO("{}", vec2);


	LOG_TRACE("Hello World!, {}");
	std::cin.get();
	return 0;
}