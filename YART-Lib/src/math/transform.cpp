#include "math/transform.h"

namespace yart
{
    Transform Inverse(const Transform& transform)
    {
        return Transform(transform.m_Inv, transform.m_Mat);
    }

    Transform Transpose(const Transform& transform)
    {
        return Transform(Transpose(transform.m_Mat), Transpose(transform.m_Inv));
    }

    Transform Translate(const Vector3f& delta)
    {
        // clang-format off
		Matrix4x4 mat(1, 0, 0, delta.x,
					  0, 1, 0, delta.y,
					  0, 0, 1, delta.z,
					  0, 0, 0, 1);
		Matrix4x4 inv(1, 0, 0, -delta.x,
					  0, 1, 0, -delta.y,
					  0, 0, 1, -delta.z,
					  0, 0, 0, 1);
        // clang-format on
        return Transform(mat, inv);
    }

    Transform Scale(const Vector3f& scale)
    {
        // clang-format off
		Matrix4x4 mat(scale.x, 0, 0, 0,
					  0	,scale.y, 0, 0,
					  0, 0, scale.z, 0,
					  0, 0, 0, 1);
		Matrix4x4 inv(1 / scale.x, 0, 0, 0,
					  0, 1 / scale.y, 0, 0,
					  0, 0, 1 / scale.z, 0,
					  0, 0, 0, 1);
        // clang-format on
        return Transform(mat, inv);
    }

    Transform RotateX(real theta)
    {
        real sinTheta = std::sin(Radians(theta));
        real cosTheta = std::cos(Radians(theta));
        // clang-format off
		Matrix4x4 m(1, 0, 0, 0,
					0, cosTheta, -sinTheta, 0,
					0, sinTheta, cosTheta, 0,
					0, 0, 0, 1);
        // clang-format on
        return Transform(m, Transpose(m));
    }

    Transform RotateY(real theta)
    {
        real sinTheta = std::sin(Radians(theta));
        real cosTheta = std::cos(Radians(theta));
        // clang-format off
		Matrix4x4 m(cosTheta, 0, sinTheta, 0,
					0, 1, 0, 0,
					-sinTheta, 0, cosTheta, 0,
					0, 0, 0, 1);
        // clang-format on
        return Transform(m, Transpose(m));
    }

    Transform RotateZ(real theta)
    {
        real sinTheta = std::sin(Radians(theta));
        real cosTheta = std::cos(Radians(theta));
        // clang-format off
		Matrix4x4 m(cosTheta, -sinTheta, 0, 0,
					sinTheta, cosTheta, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);
        // clang-format on
        return Transform(m, Transpose(m));
    }

    Transform Rotate(real theta, const Vector3f& axis)
    {
        Vector3f a = Normalize(axis);
        real sinTheta = std::sin(Radians(theta));
        real cosTheta = std::cos(Radians(theta));
        Matrix4x4 m;

        m.m[0][0] = a.x * a.x + (1 - a.x * a.x) * cosTheta;
        m.m[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
        m.m[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
        m.m[0][3] = 0;

        m.m[1][0] = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
        m.m[1][1] = a.y * a.y + (1 - a.y * a.y) * cosTheta;
        m.m[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
        m.m[1][3] = 0;

        m.m[2][0] = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
        m.m[2][1] = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
        m.m[2][2] = a.z * a.z + (1 - a.z * a.z) * cosTheta;
        m.m[2][3] = 0;

        return Transform(m, Transpose(m));
    }

    Transform LookAt(const Vector3f& pos, const Vector3f& look, const Vector3f& up)
    {
        Matrix4x4 cameraToWorld;
        // Initialize fourth column of viewing matrix
        cameraToWorld.m[0][3] = pos.x;
        cameraToWorld.m[1][3] = pos.y;
        cameraToWorld.m[2][3] = pos.z;
        cameraToWorld.m[3][3] = 1;

        // Initialize first three columns of viewing matrix
        Vector3f dir = Normalize(look - pos);
        if (NormSquared(Cross(Normalize(up), dir)) == 0)
        {
            LOG_ERROR("\"up\" vector ({}, {}, {}) and viewing direction ({}, {}, {}) "
                      "passed to LookAt are pointing in the same direction.  Using "
                      "the identity transformation.",
                      up.x, up.y, up.z, dir.x, dir.y, dir.z);
            return Transform();
        }
        Vector3f right = Normalize(Cross(Normalize(up), dir));
        Vector3f newUp = Cross(dir, right);
        cameraToWorld.m[0][0] = right.x;
        cameraToWorld.m[1][0] = right.y;
        cameraToWorld.m[2][0] = right.z;
        cameraToWorld.m[3][0] = 0.;
        cameraToWorld.m[0][1] = newUp.x;
        cameraToWorld.m[1][1] = newUp.y;
        cameraToWorld.m[2][1] = newUp.z;
        cameraToWorld.m[3][1] = 0.;
        cameraToWorld.m[0][2] = dir.x;
        cameraToWorld.m[1][2] = dir.y;
        cameraToWorld.m[2][2] = dir.z;
        cameraToWorld.m[3][2] = 0.;
        return Transform(Inverse(cameraToWorld), cameraToWorld);
    }

    Transform Orthographic(real zNear, real zFar)
    {
        return Scale({1, 1, 1 / (zFar - zNear)}) * Translate({0, 0, -zNear});
    }

    Transform Transform::operator*(const Transform& other) const
    {
        return Transform(m_Mat * other.m_Mat, other.m_Inv * m_Inv);
    }

    bool Transform::operator==(const Transform& other) const
    {
        return m_Mat == other.m_Mat;
    }

    bool Transform::operator!=(const Transform& other) const
    {
        return !(*this == other);
    }

    bool Transform::IsIdentity() const
    {
        return m_Mat == Matrix4x4{};
    }

    bool Transform::SwapsHandedness() const
    {
        real det = m_Mat.m[0][0] * (m_Mat.m[1][1] * m_Mat.m[2][2] - m_Mat.m[1][2] * m_Mat.m[2][1]) -
                   m_Mat.m[0][1] * (m_Mat.m[1][0] * m_Mat.m[2][2] - m_Mat.m[1][2] * m_Mat.m[2][0]) +
                   m_Mat.m[0][2] * (m_Mat.m[1][0] * m_Mat.m[2][1] - m_Mat.m[1][1] * m_Mat.m[2][0]);
        return det < 0;
    }

    Bounds3f Transform::AppBB(const Bounds3f& bb) const
    {
        const Transform& M = *this;
        Vector3f base = M.AppVec(bb.m_MinBound);

        Vector3f x = M.AppVec(bb.Diagonal() * Vector3f{1, 0, 0});
        Vector3f y = M.AppVec(bb.Diagonal() * Vector3f{0, 1, 0});
        Vector3f z = M.AppVec(bb.Diagonal() * Vector3f{0, 0, 1});

        Bounds3f ret(base);
        ret = Union(ret, base + x);
        ret = Union(ret, base + y);
        ret = Union(ret, base + z);
        ret = Union(ret, base + x + y);
        ret = Union(ret, base + x + z);
        ret = Union(ret, base + y + z);
        ret = Union(ret, base + x + y + z);
        return ret;
    }

}