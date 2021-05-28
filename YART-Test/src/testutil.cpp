#include "testutil.h"
using namespace Catch;

bool Vector3fAreEqual(const Vector3f& v1, const Vector3f& v2)
{
	for (int i = 0; i < 3; i++)
	{
		if (v1[i] != Approx(v2[i]))
		{
			return false;
		}
	}
	return true;
}

bool Vector2fAreEqual(const Vector2f& v1, const Vector2f& v2)
{
	for (int i = 0; i < 2; i++)
	{
		if (v1[i] != Approx(v2[i]))
		{
			return false;
		}
	}
	return true;
}

bool MatrixAreEqual(const Matrix4x4& m1, const Matrix4x4& m2)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (m1.m[i][j] != Approx(m2.m[i][j]))
				return false;
		}
	}
	return true;
}