#include "matrix.h"
#include "pch.h"

namespace yart
{
	Matrix4x4::Matrix4x4()
	{
		memset(m, 0, 16 * sizeof(Float));
		for (int i = 0; i < 4; i++)
		{
			m[i][i] = 1;
		}
	}

	Matrix4x4::Matrix4x4(const Float mat[4][4])
	{
		memcpy(m, mat, 16 * sizeof(Float));
	}
	Matrix4x4::Matrix4x4(Float t00, Float t01, Float t02, Float t03,
						 Float t10, Float t11, Float t12, Float t13,
						 Float t20, Float t21, Float t22, Float t23,
						 Float t30, Float t31, Float t32, Float t33)
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
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
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
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
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

	// Use Gauss–Jordan elimination
	Matrix4x4 Inverse(const Matrix4x4& mat)
	{
		int indxc[4], indxr[4];
		int ipiv[4] = {0, 0, 0, 0};
		Float minv[4][4];
		memcpy(minv, mat.m, 4 * 4 * sizeof(Float));
		for (int i = 0; i < 4; i++)
		{
			int irow = 0, icol = 0;
			Float big = 0.0f;
			// Choose pivot
			for (int j = 0; j < 4; j++)
			{
				if (ipiv[j] != 1)
				{
					for (int k = 0; k < 4; k++)
					{
						if (ipiv[k] == 0)
						{
							if (std::abs(minv[j][k]) >= big)
							{
								big = Float(std::abs(minv[j][k]));
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
				for (int k = 0; k < 4; ++k)
					std::swap(minv[irow][k], minv[icol][k]);
			}
			indxr[i] = irow;
			indxc[i] = icol;

			if (minv[icol][icol] == 0.0f)
			{
				LOG_ERROR("m is non invertable");
			}

			// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
			Float pivinv = 1. / minv[icol][icol];
			minv[icol][icol] = 1.;
			for (int j = 0; j < 4; j++)
				minv[icol][j] *= pivinv;

			// Subtract this row from others to zero out their columns
			for (int j = 0; j < 4; j++)
			{
				if (j != icol)
				{
					Float save = minv[j][icol];
					minv[j][icol] = 0;
					for (int k = 0; k < 4; k++)
						minv[j][k] -= minv[icol][k] * save;
				}
			}
		}
		// Swap columns to reflect permutation
		for (int j = 3; j >= 0; j--)
		{
			if (indxr[j] != indxc[j])
			{
				for (int k = 0; k < 4; k++)
					std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
			}
		}
		return Matrix4x4(minv);
	}
}