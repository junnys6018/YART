#include "pch.h"
#include "core/yart.h"

using namespace yart;

int main(int argc, char** argv)
{
	yart::Initialize();

	Transform t = RotateX(45);
	Bounds3f bb({0, 0, 0}, {10, 10, 10});
	Ray ray({1, 1, 1}, Normalize(Vector3f{2, 3, 4}));

	float t1, t2;
	LOG_INFO("{}", bb.IntersectRay(ray, t1, t2));

	LOG_INFO("{}", t.AppBB(bb));

	std::cin.get();
	return 0;
}