#pragma once
#include <catch_amalgamated.hpp>
#include <yart.h>
using namespace yart;

bool Vector3fAreEqual(const Vector3f& v1, const Vector3f& v2);
bool Vector2fAreEqual(const Vector2f& v1, const Vector2f& v2);
bool MatrixAreEqual(const Matrix4x4& m1, const Matrix4x4& m2);