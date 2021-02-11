#pragma once
#include "pch.h"

namespace yart
{
	class Matrix4x4
	{
	public:
		// Default construct the identity
		Matrix4x4();

		Matrix4x4(const Float mat[4][4]);

		Matrix4x4(Float t00, Float t01, Float t02, Float t03,
				  Float t10, Float t11, Float t12, Float t13,
				  Float t20, Float t21, Float t22, Float t23,
				  Float t30, Float t31, Float t32, Float t33);

		bool operator==(const Matrix4x4& other) const;
		bool operator!=(const Matrix4x4& other) const;

		Matrix4x4 operator*(const Matrix4x4& other) const;

	public:
		Float m[4][4];
	};

	Matrix4x4 Transpose(const Matrix4x4& mat);

	Matrix4x4 Inverse(const Matrix4x4& mat);
}