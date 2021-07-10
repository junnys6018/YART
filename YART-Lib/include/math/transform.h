#pragma once
#include "core/interaction.h"
#include "core/yart.h"
#include "math/boundingbox.h"
#include "math/matrix.h"
#include "math/ray.h"
#include "math/vector.h"

namespace yart
{
    class Transform
    {
    public:
        Transform() = default;

        Transform(const real mat[4][4]) : m_Mat(mat), m_Inv(Inverse(m_Mat))
        {
        }

        Transform(const Matrix4x4& mat) : m_Mat(mat), m_Inv(Inverse(m_Mat))
        {
        }

        // Provide the inverse to save calculation
        Transform(const Matrix4x4& mat, const Matrix4x4& inv) : m_Mat(mat), m_Inv(inv)
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
        Vector3<T> AppPoint(const Vector3<T>& pt) const
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

        // Apply transform to bounding box
        Bounds3f AppBB(const Bounds3f& bb) const;

        // Apply transform to ray
        Ray AppRay(const Ray& ray) const
        {
            // TODO: offset ray origin to edge of error bounds
            Vector3f o = this->AppPoint(ray.o);
            Vector3f d = this->AppVec(ray.d);

            return Ray(o, d, ray.m_Tmax, ray.m_Time);
        }

        SurfaceInteraction AppSI(const SurfaceInteraction& si) const
        {
            // TODO: Error bounds
            const Transform& t = *this;
            SurfaceInteraction ret;

            ret.m_Point = t.AppPoint(si.m_Point);
            ret.m_Normal = Normalize(t.AppNorm(si.m_Normal));
            ret.m_PtError = si.m_PtError;
            ret.m_wo = t.AppVec(si.m_wo);
            ret.m_Time = si.m_Time;
            ret.m_uv = si.m_uv;
            ret.m_dpdu = t.AppVec(si.m_dpdu);
            ret.m_dpdv = t.AppVec(si.m_dpdv);
            ret.m_dndv = t.AppNorm(si.m_dndv);
            ret.m_dndu = t.AppNorm(si.m_dndv);
            ret.m_Geometry = si.m_Geometry;

            ret.m_Shading.m_Normal = Normalize(t.AppNorm(si.m_Shading.m_Normal));
            ret.m_Shading.m_Normal = FaceForward(ret.m_Shading.m_Normal, ret.m_Normal);
            ret.m_Shading.m_dpdu = t.AppVec(si.m_Shading.m_dpdu);
            ret.m_Shading.m_dpdv = t.AppVec(si.m_Shading.m_dpdv);
            ret.m_Shading.m_dndv = t.AppNorm(si.m_Shading.m_dndv);
            ret.m_Shading.m_dndu = t.AppNorm(si.m_Shading.m_dndv);

            return ret;
        }

    private:
        Matrix4x4 m_Mat, m_Inv;
    };

    Transform Translate(const Vector3f& delta);
    Transform Scale(const Vector3f& scale);
    Transform RotateX(real theta);
    Transform RotateY(real theta);
    Transform RotateZ(real theta);
    Transform Rotate(real theta, const Vector3f& axis);
    Transform LookAt(const Vector3f& pos, const Vector3f& look, const Vector3f& up);
    Transform Orthographic(real zNear, real zFar);
}
