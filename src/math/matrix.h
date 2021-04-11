#pragma once
#include "pch.h"

namespace yart
{
	class Matrix4x4
	{
	public:
		// Default construct the identity
		Matrix4x4();

		Matrix4x4(const real mat[4][4]);

		Matrix4x4(real t00, real t01, real t02, real t03,
				  real t10, real t11, real t12, real t13,
				  real t20, real t21, real t22, real t23,
				  real t30, real t31, real t32, real t33);

		bool operator==(const Matrix4x4& other) const;
		bool operator!=(const Matrix4x4& other) const;

		Matrix4x4 operator*(const Matrix4x4& other) const;

	public:
		real m[4][4];
	};

	Matrix4x4 Transpose(const Matrix4x4& mat);

	Matrix4x4 Inverse(const Matrix4x4& mat);
}