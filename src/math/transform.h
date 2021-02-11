#pragma once
#include "pch.h"

namespace yart
{
	class Transform
	{
	public:
		Transform() = default;

		Transform(const Float mat[4][4])
			: m_Mat(mat), m_Inv(Inverse(m_Mat))
		{
		}

		Transform(const Matrix4x4& mat)
			: m_Mat(mat), m_Inv(Inverse(m_Mat))
		{
		}

		// Provide the inverse to save calculation
		Transform(const Matrix4x4& mat, const Matrix4x4& inv)
			: m_Mat(mat), m_Inv(inv)
		{
		}

		friend Transform Inverse(const Transform& transform);
		friend Transform Transpose(const Transform& transform);

		Transform operator*(const Transform& other) const;

		bool operator==(const Transform& other) const;
		bool operator!=(const Transform& other) const;
		bool IsIdentity() const;

		bool SwapsHandedness() const;

		// Apply transform to vector
		template <typename T>
		Vector3<T> AppVec(const Vector3<T>& vec) const
		{
			T x = vec.x, y = vec.y, z = vec.z;
			return Vector3<T>(m_Mat.m[0][0] * x + m_Mat.m[0][1] * y + m_Mat.m[0][2] * z,
							  m_Mat.m[1][0] * x + m_Mat.m[1][1] * y + m_Mat.m[1][2] * z,
							  m_Mat.m[2][0] * x + m_Mat.m[2][1] * y + m_Mat.m[2][2] * z);
		}

		// Apply transform to point
		template <typename T>
		Vector3<T> AppPt(const Vector3<T>& pt) const
		{
			T x = pt.x, y = pt.y, z = pt.z;
			T xp = m_Mat.m[0][0] * x + m_Mat.m[0][1] * y + m_Mat.m[0][2] * z + m_Mat.m[0][3];
			T yp = m_Mat.m[1][0] * x + m_Mat.m[1][1] * y + m_Mat.m[1][2] * z + m_Mat.m[1][3];
			T zp = m_Mat.m[2][0] * x + m_Mat.m[2][1] * y + m_Mat.m[2][2] * z + m_Mat.m[2][3];
			T wp = m_Mat.m[3][0] * x + m_Mat.m[3][1] * y + m_Mat.m[3][2] * z + m_Mat.m[3][3];
			if (wp == 1)
				return Vector3<T>(xp, yp, zp);
			else
				return Vector3<T>(xp, yp, zp) / wp;
		}

		// Apply transform to normal
		template <typename T>
		Vector3<T> AppNorm(const Vector3<T>& norm) const
		{
			T x = norm.x, y = norm.y, z = norm.z;
			return Vector3<T>(m_Inv.m[0][0] * x + m_Inv.m[1][0] * y + m_Inv.m[2][0] * z,
							  m_Inv.m[0][1] * x + m_Inv.m[1][1] * y + m_Inv.m[2][1] * z,
							  m_Inv.m[0][2] * x + m_Inv.m[1][2] * y + m_Inv.m[2][2] * z);
		}

	private:
		Matrix4x4 m_Mat, m_Inv;
	};

	Transform Translate(const Vector3f& delta);
	Transform Scale(const Vector3f& scale);
	Transform RotateX(Float theta);
	Transform RotateY(Float theta);
	Transform RotateZ(Float theta);
	Transform Rotate(Float theta, const Vector3f& axis);
	Transform LookAt(const Vector3f& pos, const Vector3f& look, const Vector3f& up);
}
