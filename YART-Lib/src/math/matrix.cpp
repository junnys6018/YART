#include "math/matrix.h"

namespace yart
{
	Matrix4x4::Matrix4x4()
	{
		std::memset(m, 0, 16 * sizeof(real));
		for (i32 i = 0; i < 4; i++)
		{
			m[i][i] = 1;
		}
	}

	Matrix4x4::Matrix4x4(const real mat[4][4])
	{
		std::memcpy(m, mat, 16 * sizeof(real));
	}

	Matrix4x4::Matrix4x4(real t00, real t01, real t02, real t03,
						 real t10, real t11, real t12, real t13,
						 real t20, real t21, real t22, real t23,
						 real t30, real t31, real t32, real t33)
	{
		m[0][0] = t00;
		m[0][1] = t01;
		m[0][2] = t02;
		m[0][3] = t03;

		m[1][0] = t10;
		m[1][1] = t11;
		m[1][2] = t12;
		m[1][3] = t13;

		m[2][0] = t20;
		m[2][1] = t21;
		m[2][2] = t22;
		m[2][3] = t23;

		m[3][0] = t30;
		m[3][1] = t31;
		m[3][2] = t32;
		m[3][3] = t33;
	}

	bool Matrix4x4::operator==(const Matrix4x4& other) const
	{
		for (i32 i = 0; i < 4; i++)
		{
			for (i32 j = 0; j < 4; j++)
			{
				if (m[i][j] != other.m[i][j])
				{
					return false;
				}
			}
		}
		return true;
	}
	bool Matrix4x4::operator!=(const Matrix4x4& other) const
	{
		return !(*this == other);
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
	{
		Matrix4x4 ret;
		for (i32 i = 0; i < 4; i++)
		{
			for (i32 j = 0; j < 4; j++)
			{
				ret.m[i][j] = m[i][0] * other.m[0][j] +
							  m[i][1] * other.m[1][j] +
							  m[i][2] * other.m[2][j] +
							  m[i][3] * other.m[3][j];
			}
		}

		return ret;
	}

	Matrix4x4 Transpose(const Matrix4x4& mat)
	{
		return Matrix4x4(mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
						 mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
						 mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
						 mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
	}

	// Use Gauss�Jordan elimination
	Matrix4x4 Inverse(const Matrix4x4& mat)
	{
		i32 indxc[4], indxr[4];
		i32 ipiv[4] = {0, 0, 0, 0};
		real minv[4][4];
		std::memcpy(minv, mat.m, 4 * 4 * sizeof(real));
		for (i32 i = 0; i < 4; i++)
		{
			i32 irow = 0, icol = 0;
			real big = 0.0f;
			// Choose pivot
			for (i32 j = 0; j < 4; j++)
			{
				if (ipiv[j] != 1)
				{
					for (i32 k = 0; k < 4; k++)
					{
						if (ipiv[k] == 0)
						{
							if (std::abs(minv[j][k]) >= big)
							{
								big = real(std::abs(minv[j][k]));
								irow = j;
								icol = k;
							}
						}

						if (ipiv[k] > 1)
						{
							LOG_ERROR("m is non invertable");
						}
					}
				}
			}
			++ipiv[icol];
			// Swap rows _irow_ and _icol_ for pivot
			if (irow != icol)
			{
				for (i32 k = 0; k < 4; ++k)
					std::swap(minv[irow][k], minv[icol][k]);
			}
			indxr[i] = irow;
			indxc[i] = icol;

			if (minv[icol][icol] == 0.0f)
			{
				LOG_ERROR("m is non invertable");
			}

			// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
			real pivinv = (real)1.0 / minv[icol][icol];
			minv[icol][icol] = 1.;
			for (i32 j = 0; j < 4; j++)
				minv[icol][j] *= pivinv;

			// Subtract this row from others to zero out their columns
			for (i32 j = 0; j < 4; j++)
			{
				if (j != icol)
				{
					real save = minv[j][icol];
					minv[j][icol] = 0;
					for (i32 k = 0; k < 4; k++)
						minv[j][k] -= minv[icol][k] * save;
				}
			}
		}
		// Swap columns to reflect permutation
		for (i32 j = 3; j >= 0; j--)
		{
			if (indxr[j] != indxc[j])
			{
				for (i32 k = 0; k < 4; k++)
					std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
			}
		}
		return Matrix4x4(minv);
	}
}